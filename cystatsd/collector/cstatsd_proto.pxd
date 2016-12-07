from libcpp.string cimport string
from libcpp.vector cimport vector

cdef extern from "./statsd_proto.hpp" namespace "statsd_proto":
    cdef cppclass MetricCollector:
        MetricCollector()
        MetricCollector(size_t)
        void pushGauge(const string &name, int value)
        void pushGaugeDelta(const string &name, int value)
        void pushTimer(const string &name, int value)
        void pushCounter(const string &name, int value)
        void pushSet(const string &name, int value)
        void pushGauge(const string &name, int value, float rate)
        void pushGaugeDelta(const string &name, int value, float rate)
        void pushTimer(const string &name, int value, float rate)
        void pushCounter(const string &name, int value, float rate)
        void pushSet(const string &name, int value, float rate)
        vector[string] flush()

