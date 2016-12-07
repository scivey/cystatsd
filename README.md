## cystatsd

A c++/cython encoder for batches of statsd metrics.

The main class is `cystatsd.MetricCollector`, which accepts metrics and converts them into packets for batch submission to a StatsD server.  Target packet size is configurable.

This doesn't make any network calls; it just does encoding.

This also doesn't currently do anything clever with stats, like collapsing submissions for the same counter.  There's a potential for more optimization there.

### Install

It requires Cython to build:
```bash
pip install cython
```

Then:
```bash
pip install cystatsd
```

### Example

```python
from cystatsd import MetricCollector
collector = MetricCollector()
collector.push_timer("some_timer", 200)
collector.push_counter("dogs", 2)
collector.push_counter("fish", 5, rate=0.1)
result = collector.flush()
print(result)
```

Outputs:
```python
[b"some_timer:200|ms\ndogs:2|c\nfish:5|c|@0.10"]
```

### Contributors
* @leftys added support for set types, gauge deltas and counter decrements. (1.1.0)
* @scivey initial implementation

### License
MIT
