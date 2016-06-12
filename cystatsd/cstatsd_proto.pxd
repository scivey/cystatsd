from libcpp.string cimport string
from libcpp.vector cimport vector

from . cimport cpp_help
cdef extern from "./statsd_proto.hpp" namespace "statsd_proto":
    cdef cppclass MetricType:
        pass


cdef extern from "./statsd_proto.hpp" namespace "statsd_proto::MetricType":
    cdef MetricType TIMER
    cdef MetricType GAUGE
    cdef MetricType COUNTER
    cdef MetricType SET

cdef extern from "./statsd_proto.hpp" namespace "statsd_proto":
    cdef cppclass Metric:
        Metric()
        Metric(MetricType, string, int)
        size_t encodeTo(cpp_help.ostringstream&)

    cdef cppclass SampledMetric:
        SampledMetric()
        SampledMetric(Metric, float)
        SampledMetric(Metric)
        size_t encodeTo(cpp_help.ostringstream&)
        string encode()

    cdef cppclass MetricCollector:
        MetricCollector()
        MetricCollector(size_t)
        void pushGauge(const string &name, int value)
        void pushTimer(const string &name, int value)
        void pushCounter(const string &name, int value)
        void pushGauge(const string &name, int value, float rate)
        void pushTimer(const string &name, int value, float rate)
        void pushCounter(const string &name, int value, float rate)
        void pushSet(const string &name, int value, float rate)
        vector[string] flush()

