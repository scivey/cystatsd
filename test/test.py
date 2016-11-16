
# this has to be run in a virtualenv after
# installing with setup.py, due to need for compilation

from cystatsd import MetricCollector
import unittest

def encode_timer(name, val, rate=1.0):
    collector = MetricCollector()
    collector.push_timer(name, val, rate)
    result = list(collector.flush())
    assert len(result) == 1
    return result[0]

def encode_counter(name, val, rate=1.0):
    collector = MetricCollector()
    collector.push_counter(name, val, rate)
    result = list(collector.flush())
    assert len(result) == 1
    return result[0]

def encode_gauge(name, val, rate=1.0, delta=False):
    collector = MetricCollector()
    collector.push_gauge(name, val, rate, delta)
    result = list(collector.flush())
    assert len(result) == 1
    return result[0]

def encode_set(name, val, rate=1.0):
    collector = MetricCollector()
    collector.push_set(name, val, rate)
    result = list(collector.flush())
    assert len(result) == 1
    return result[0]

class BasicCollectorTests(unittest.TestCase):

    def test_timer(self):
        self.assertEqual(b"foo:200|ms", encode_timer("foo", 200))
        self.assertEqual(b"bar:215|ms|@0.50", encode_timer("bar", 215, 0.5))
        self.assertEqual(b"bar:215|ms", encode_timer("bar", 215, 1.5))

    def test_counter(self):
        self.assertEqual(b"foo:200|c", encode_counter("foo", 200))
        self.assertEqual(b"bar:215|c|@0.50", encode_counter("bar", 215, 0.5))
        self.assertEqual(b"bar:215|c", encode_counter("bar", 215, 1.5))

    def test_gauge(self):
        self.assertEqual(b"foo:200|g", encode_gauge("foo", 200))
        self.assertEqual(b"foo:+200|g", encode_gauge("foo", 200, delta=True))
        self.assertEqual(b"foo:-200|g", encode_gauge("foo", -200, delta=True))
        self.assertEqual(b"bar:215|g|@0.50", encode_gauge("bar", 215, 0.5))
        self.assertEqual(b"bar:215|g", encode_gauge("bar", 215, 1.5))

    def test_set(self):
        self.assertEqual(b"foo:200|s", encode_set("foo", 200))
        self.assertEqual(b"bar:215|s|@0.50", encode_set("bar", 215, 0.5))
        self.assertEqual(b"bar:215|s", encode_set("bar", 215, 1.5))

class BatchingTests(unittest.TestCase):
    def test_batching_1(self):
        MTU = 24
        collector = MetricCollector(MTU)
        for i in range(8):
            met_name = "m%i" % i
            collector.push_gauge(met_name, 100 + i)
        result = list(collector.flush())
        self.assertEqual([
            b"m0:100|g\nm1:101|g",
            b"m2:102|g\nm3:103|g",
            b"m4:104|g\nm5:105|g",
            b"m6:106|g\nm7:107|g"
        ], result)

    def test_batching_1(self):
        MTU = 512
        collector = MetricCollector(MTU)
        for i in range(8):
            met_name = "m%i" % i
            collector.push_gauge(met_name, 100 + i)
        result = list(collector.flush())
        print('RESULT', result)
        self.assertEqual([
            b"m0:100|g\nm1:101|g\n"
            b"m2:102|g\nm3:103|g\n"
            b"m4:104|g\nm5:105|g\n"
            b"m6:106|g\nm7:107|g"
        ], result)
