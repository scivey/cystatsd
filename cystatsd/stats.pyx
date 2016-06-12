from libcpp.string cimport string
from libcpp.vector cimport vector
from libc.stdio cimport sprintf
from . cimport cpp_help
from .cpp_help cimport ostringstream
#from . cimport Metric, MetricType, SampledMetric

from .cstatsd_proto cimport (
    Metric, MetricType, TIMER, COUNTER, GAUGE, SET, SampledMetric,
    MetricCollector
)
#cdef enum MetricType:
#    TIMER = 1
#    COUNTER = 2
#    GAUGE = 3
#    SET = 4

#cdef class Metric:
#    cdef:
#        MetricType metric_type
#        string name
#        int value

#    def __cinit__(self, MetricType mt, string name, int val):
#        self.metric_type = mt
#        self.name = name
#        self.value = val

#cdef int encode_metric_to(Metric metric, ostringstream &oss_out):
#    cdef int bytes_written = 0
#    oss_out.write(metric.name.c_str(), metric.name.size())
#    bytes_written += metric.name.size()
#    cdef char intBuff [35]
#    sprintf(intBuff, ":%i|", metric.value)
#    cdef int intBuffLen = len(intBuff)
#    oss_out.write(intBuff, intBuffLen)
#    bytes_written += intBuffLen
#    if metric.metric_type == TIMER:
#        oss_out.write("ms", 2)
#        bytes_written += 2
#    elif metric.metric_type == GAUGE:
#        oss_out.write("g", 1)
#        bytes_written += 1
#    elif metric.metric_type == SET:
#        oss_out.write("s", 1)
#        bytes_written += 1
#    elif metric.metric_type == COUNTER:
#        oss_out.write("c", 1)
#        bytes_written += 1

#cdef string encode_metric(Metric metric):
#    cdef ostringstream oss
#    encode_metric_to(metric, oss)
#    return oss.str()



#cdef class SampledMetric:
#    cdef:
#        Metric metric
#        float sample_rate

#    def __cinit__(self, Metric metric, float sample_rate_):
#        self.metric = metric
#        self.sample_rate = sample_rate_



#cdef encode_sampled_metric_to(SampledMetric metric, ostringstream &result):
#    cdef int bytes_written = encode_metric_to(metric.metric, result)
#    cdef char rateBuff[8]
#    cdef int rateLen = len(rateBuff)
#    if metric.sample_rate < 1.0:
#        sprintf(rateBuff, "|@%.2f", metric.sample_rate)
#        result.write(rateBuff, rateLen)
#        bytes_written += rateLen
#    return bytes_written


#cdef class MetricCollector:
#    cdef:
#        vector[SampledMetric] metrics
#        int mtuSize = 512

#    cdef bint empty(self):
#        return self.metrics.empty()

#    cdef _push_metric(self, Metric metric, float rate=1.0):
#        cdef SampledMetric smet = SampledMetric(metric, rate)
#        self.metrics.push_back(smet)

#    cdef push_counter(self, string name, int value, float rate=1.0):
#        cdef Metric met = Metric(COUNTER, name, value)
#        self._push_metric(met, rate)

#    cdef push_timer(self, string name, int value, float rate=1.0):
#        cdef Metric met = Metric(TIMER, name, value)
#        self._push_metric(met, rate)


#cdef string encode_sampled_metric(SampledMetric metric):
#    cdef ostringstream oss
#    encode_sampled_metric_to(metric, oss)
#    return oss.str()

def doit():
    cdef MetricCollector collector = MetricCollector(512)
    cdef int i = 0
    while i < 50:
        collector.pushTimer(b"something", 500)
        collector.pushCounter(b"foo", 2)
        collector.pushCounter(b"bar", 5, 0.1)

        i += 1
    cdef vector[string] messages = collector.flush()
    for msg in messages:
        print(msg)
