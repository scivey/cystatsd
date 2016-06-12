#include <string>
#include <cstdint>
#include <sstream>
#include <deque>
#include <vector>

namespace statsd_proto {

enum class MetricType {
  TIMER,
  COUNTER,
  GAUGE,
  SET,
  NONE
};


class Metric {
 protected:
  MetricType metricType_ {MetricType::NONE};
  std::string name_ {""};
  int64_t value_ {0};
 public:
  Metric();
  Metric(MetricType mtype, std::string name, int64_t val);
  size_t encodeTo(std::ostringstream&);
};

class SampledMetric {
 protected:
  Metric metric_;
  float rate_ {1.0};
 public:
  SampledMetric();
  bool isSampled() const;
  SampledMetric(Metric met, float rate);
  SampledMetric(MetricType met, const std::string &name, int64_t val, float rate);
  SampledMetric(Metric met);
  std::string encode();
  size_t encodeTo(std::ostringstream&);
};

class MetricCollector {
 protected:
  std::deque<SampledMetric> metrics_;
  size_t mtuSize_ {512};
 public:
  MetricCollector();
  MetricCollector(size_t);
  size_t count() const;
  bool empty() const;
  std::vector<std::string> flush();
  void pushTimer(const std::string& name, int64_t value, float rate = 1.0);
  void pushCounter(const std::string& name, int64_t value, float rate = 1.0);
  void pushGauge(const std::string& name, int64_t value, float rate = 1.0);
  void pushSet(const std::string& name, int64_t value, float rate = 1.0);
};


}