#include "statsd_proto.hpp"
#include <cstring>
#include <inttypes.h>

using namespace std;

namespace statsd_proto {

BufferHandle::BufferHandle(char *data, size_t maxSize)
  : data_(data), maxSize_(maxSize) {}

void BufferHandle::write(const char *msg, size_t nchar) {
  size_t remaining = (maxSize_ - 1) - position_;
  if (nchar > remaining) {
    nchar = remaining;
  }
  char *targetPtr = data_ + position_;
  memcpy((void*) targetPtr, msg, nchar);
  position_ += nchar;
  data_[position_] = '\0';
}

bool BufferHandle::empty() const {
  return size() == 0;
}

void BufferHandle::clear() {
  position_ = 0;
  data_[0] = '\0';
}

size_t BufferHandle::size() const {
  return position_;
}

void BufferHandle::write(const std::string& str) {
  write(str.c_str(), str.size());
}

const char* BufferHandle::data() const {
  return data_;
}



Metric::Metric(){}

Metric::Metric(const Metric & met)
  : metricType_(met.metricType_), name_(met.name_), value_(met.value_) {}

Metric::Metric(MetricType mtype, std::string name, int64_t val)
  : metricType_(mtype), name_(name), value_(val) {}

void Metric::formatValue(char * buffer) const {
  if (metricType_ == MetricType::GAUGE_DELTA) {
    sprintf(buffer, ":%+" PRId64 "|", value_);
  } else {
    sprintf(buffer, ":%" PRId64 "|", value_);
  }
}

void Metric::encodeTo(BufferHandle *buff) {
  if (metricType_ == MetricType::NONE) {
    return;
  }
  buff->write(name_);
  char intBuff[35];
  this->formatValue(intBuff);
  buff->write(intBuff, strlen(intBuff));
  switch (metricType_) {
    case MetricType::COUNTER:
      buff->write("c", 1);
      break;
    case MetricType::GAUGE:
    case MetricType::GAUGE_DELTA:
      buff->write("g", 1);
      break;
    case MetricType::TIMER:
      buff->write("ms", 2);
      break;
    case MetricType::SET:
      buff->write("s", 1);
      break;
    case MetricType::NONE:
      ; // we already checked for this earlier, but clang doesn't know that.
  }
}

MetricType Metric::getMetricType() const {
  return metricType_;
}


FloatMetric::FloatMetric(const FloatMetric & met) : Metric(met), floatValue_(met.floatValue_) {}

FloatMetric::FloatMetric(MetricType mtype, std::string name, float val)
  : Metric(mtype, name, 0), floatValue_(val) {}

void FloatMetric::formatValue(char * buffer) const {
  if (metricType_ == MetricType::GAUGE_DELTA) {
    sprintf(buffer, ":%+.6f|", floatValue_);
  } else {
    sprintf(buffer, ":%.6f|", floatValue_);
  }
}


SampledMetric::~SampledMetric() {
  if (metric_ != NULL) {
    delete metric_;
    metric_ = NULL;
  }
}

SampledMetric::SampledMetric(const SampledMetric & met) : rate_(met.rate_) {
  if (met.metric_ != NULL) {
    switch (met.metric_->getMetricType()) {
      case MetricType::COUNTER:
      case MetricType::SET:
        metric_ = new Metric(*met.metric_);
        break;

      case MetricType::GAUGE:
      case MetricType::GAUGE_DELTA:
      case MetricType::TIMER:
        metric_ = new FloatMetric(*((FloatMetric*)met.metric_));
        break;

      case MetricType::NONE:
        metric_ = NULL;
    }
  }
}

SampledMetric::SampledMetric(MetricType mtype, const string& name, int64_t value, float rate) : metric_(NULL), rate_(rate) {
  metric_ = new Metric(mtype, name, value);
}

SampledMetric::SampledMetric(MetricType mtype, const string& name, float value, float rate) : metric_(NULL), rate_(rate) {
  metric_ = new FloatMetric(mtype, name, value);
}

SampledMetric::SampledMetric() : metric_(NULL) {}

bool SampledMetric::isSampled() const {
  return rate_ < 1.0;
}

void SampledMetric::encodeTo(BufferHandle *buff) {
  if (metric_ != NULL) {
	  metric_->encodeTo(buff);
	  if (isSampled()) {
		char rateBuff[8];
		sprintf(rateBuff, "|@%.2f", rate_);
		buff->write(rateBuff, strlen(rateBuff));
	  }
  }
}




MetricCollector::MetricCollector() {}
MetricCollector::MetricCollector(size_t mtu): mtuSize_(mtu) {}

bool MetricCollector::empty() const {
  return count() == 0;
}

size_t MetricCollector::count() const {
  return metrics_.size();
}

struct MetricBuff {
  char data[256];
  MetricBuff() {
    memset((void*) data, '\0', sizeof(data));
  }
  BufferHandle getHandle() {
    return BufferHandle((char*) data, sizeof(data));
  }
};


std::vector<std::string> MetricCollector::flush() {
  std::vector<std::string> result;
  MetricBuff metricBuff;
  auto buff = metricBuff.getHandle();
  while (!empty()) {
    size_t nWritten = 0;
    ostringstream current;
    size_t bytesWritten = 0;
    if (!buff.empty()) {
      current.write(buff.data(), buff.size());
      bytesWritten += buff.size();
      nWritten++;
      buff.clear();
    }
    while (bytesWritten < mtuSize_ && !metrics_.empty()) {
      auto item = metrics_.front();
      metrics_.pop_front();
      item.encodeTo(&buff);
      if (bytesWritten + buff.size() + 1 <= mtuSize_) {
        if (nWritten > 0) {
          current.write("\n", 1);
          bytesWritten += 1;
        }
        current.write(buff.data(), buff.size());
        nWritten += 1;
        bytesWritten += buff.size();
        buff.clear();
      } else {
        break;
      }
    }
    if (bytesWritten > 0) {
      result.emplace_back(current.str());
    }
  }
  return result;
}

void MetricCollector::pushTimer(const string& name, float val, float rate) {
  metrics_.emplace_back(MetricType::TIMER, name, val, rate);
}

void MetricCollector::pushCounter(const string& name, int64_t val, float rate) {
  metrics_.emplace_back(MetricType::COUNTER, name, val, rate);
}

void MetricCollector::pushGauge(const string& name, float val, float rate) {
  metrics_.emplace_back(MetricType::GAUGE, name, val, rate);
}

void MetricCollector::pushGaugeDelta(const string& name, float val, float rate) {
  metrics_.emplace_back(MetricType::GAUGE_DELTA, name, val, rate);
}

void MetricCollector::pushSet(const string& name, int64_t val, float rate) {
  metrics_.emplace_back(MetricType::SET, name, val, rate);
}


} // statsd_proto
