.PHONY: clean

clean:
	rm -rf build
	rm -f cystatsd/collector/collector.cpp

test: clean
	python setup.py install
	cd test && nosetests test.py
