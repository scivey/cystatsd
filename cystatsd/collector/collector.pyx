from libcpp.string cimport string
from libcpp.vector cimport vector
from libc.stdio cimport sprintf

from .cstatsd_proto cimport MetricCollector as CPPMetricCollector

__all__ = ('MetricCollector',)

import sys

if sys.version_info[0] >= 3:
    UNICODE_TYPE = str
else:
    UNICODE_TYPE = unicode


cdef class MetricCollector:
    cdef:
        CPPMetricCollector collector

    def __init__(self, int mtu=512):
        self.collector = CPPMetricCollector(mtu)

    cpdef _push_timer(self, bytes name, float value, float rate):
        self.collector.pushTimer(name, value, rate)

    def push_timer(self, name, float value, float rate=1.0):
        if isinstance(name, UNICODE_TYPE):
            name = name.encode('utf-8')
        self._push_timer(name, value, rate)

    cpdef _push_counter(self, bytes name, int value, float rate=1.0):
        self.collector.pushCounter(name, value, rate)

    def push_counter(self, name, int value, float rate=1.0):
        if isinstance(name, UNICODE_TYPE):
            name = name.encode('utf-8')
        self._push_counter(name, value, rate)

    cpdef _push_gauge(self, bytes name, float value, float rate=1.0):
        self.collector.pushGauge(name, value, rate)

    cpdef _push_gauge_delta(self, bytes name, float value, float rate=1.0):
        self.collector.pushGaugeDelta(name, value, rate)

    def push_gauge(self, name, float value, float rate=1.0, delta = False):
        if isinstance(name, UNICODE_TYPE):
            name = name.encode('utf-8')
        if delta: 
            self._push_gauge_delta(name, value, rate)
        else:
            self._push_gauge(name, value, rate)

    cpdef _push_set(self, bytes name, int value, float rate=1.0):
        self.collector.pushSet(name, value, rate)

    def push_set(self, name, int value, float rate=1.0):
        if isinstance(name, UNICODE_TYPE):
            name = name.encode('utf-8')
        self._push_set(name, value, rate)

    def flush(self):
        cdef vector[string] flushed = self.collector.flush()
        for part in flushed:
            yield part

