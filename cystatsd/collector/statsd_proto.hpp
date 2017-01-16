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
  GAUGE_DELTA,
  SET,

  // cython has an annoying thing about stack allocation
  // and default constructors, so we use this to indicate
  // the default "invalid" state.
  NONE
};


class BufferHandle {
 protected:
  char *data_ {nullptr};
  size_t maxSize_ {0};
  size_t position_ {0};
 public:
  BufferHandle(char*, size_t);
  void write(const char*, size_t);
  void write(const std::string&);
  void clear();
  void write(BufferHandle*);
  size_t size() const;
  const char* data() const;
  bool empty() const;
};

class Metric {
 protected:
  MetricType metricType_ {MetricType::NONE};
  std::string name_ {""};
  int64_t value_ {0};
 public:
  Metric();
  Metric(MetricType mtype, std::string name, int64_t val);
  void encodeTo(BufferHandle*);
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
  void encodeTo(BufferHandle*);
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
  void pushTimer(const std::string& name, float value, float rate = 1.0);
  void pushCounter(const std::string& name, int64_t value, float rate = 1.0);
  void pushGauge(const std::string& name, float value, float rate = 1.0);
  void pushGaugeDelta(const std::string& name, float value, float rate = 1.0);
  void pushSet(const std::string& name, int64_t value, float rate = 1.0);
};


}
