#include "statsd_proto.hpp"
#include <cstring>
#include <inttypes.h>
using namespace std;

namespace statsd_proto {

Metric::Metric(){}
Metric::Metric(MetricType mtype, std::string name, int64_t val)
  : metricType_(mtype), name_(name), value_(val) {}

size_t Metric::encodeTo(ostringstream &oss) {
  size_t bytesWritten = 0;
  if (metricType_ == MetricType::NONE) {
    return bytesWritten;
  }
  oss << name_;
  bytesWritten += name_.size();
  char intBuff[35];
  sprintf(intBuff, ":%" PRIu64 "|", value_);
  size_t intBuffLen = strlen(intBuff);
  oss.write(intBuff, intBuffLen);
  bytesWritten += intBuffLen;
  switch (metricType_) {
    case MetricType::COUNTER:
      oss << "c";
      bytesWritten += 1;
      break;
    case MetricType::GAUGE:
      oss << "g";
      bytesWritten += 1;
      break;
    case MetricType::SET:
      oss << "s";
      bytesWritten += 1;
      break;
    case MetricType::TIMER:
      oss << "ms";
      bytesWritten += 2;
      break;
  }
  return bytesWritten;
}

SampledMetric::SampledMetric(Metric met): metric_(met) {}
SampledMetric::SampledMetric(Metric met, float rate)
  : metric_(met), rate_(rate) {}

SampledMetric::SampledMetric(MetricType mtype,
    const string& name, int64_t value, float rate)
  : metric_(mtype, name, value), rate_(rate) {}

SampledMetric::SampledMetric() {}

bool SampledMetric::isSampled() const {
  return rate_ < 1.0;
}

size_t SampledMetric::encodeTo(ostringstream &oss) {
  size_t bytesWritten = metric_.encodeTo(oss);
  if (isSampled()) {
    char rateBuff[8];
    sprintf(rateBuff, "|@%.2f", rate_);
    size_t rateLen = strlen(rateBuff);
    oss.write(rateBuff, rateLen);
    bytesWritten += rateLen;
  }
  return bytesWritten;
}


string SampledMetric::encode() {
  ostringstream oss;
  encodeTo(oss);
  return oss.str();
}


MetricCollector::MetricCollector() {}
MetricCollector::MetricCollector(size_t mtu): mtuSize_(mtu) {}

bool MetricCollector::empty() const {
  return count() == 0;
}

size_t MetricCollector::count() const {
  return metrics_.size();
}

std::vector<std::string> MetricCollector::flush() {
  std::vector<std::string> result;
  const size_t targetSize = mtuSize_ - 96;
  while (!empty()) {
    ostringstream current;
    size_t bytesWritten = 0;
    while (bytesWritten < targetSize) {
      auto item = metrics_.front();
      metrics_.pop_front();
      bytesWritten += item.encodeTo(current);
      if (empty()) {
        break;
      }
      if (bytesWritten < targetSize) {
        current << "\n";
        bytesWritten += 1;
      }
    }
    if (bytesWritten > 0) {
      result.emplace_back(current.str());
    }
  }
  return result;
}

void MetricCollector::pushTimer(const string& name, int64_t val, float rate) {
  metrics_.emplace_back(MetricType::TIMER, name, val, rate);
}

void MetricCollector::pushCounter(const string& name, int64_t val, float rate) {
  metrics_.emplace_back(MetricType::COUNTER, name, val, rate);
}

void MetricCollector::pushGauge(const string& name, int64_t val, float rate) {
  metrics_.emplace_back(MetricType::GAUGE, name, val, rate);
}

void MetricCollector::pushSet(const string& name, int64_t val, float rate) {
  metrics_.emplace_back(MetricType::SET, name, val, rate);
}


} // statsd_proto
