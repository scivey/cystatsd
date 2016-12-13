.PHONY: clean

clean:
	rm -rf build dist
	rm -f cystatsd/collector/collector.cpp MANIFEST *.egg-info *.tar.gz

test: clean
	python setup.py install
	cd test && nosetests test.py

release: clean
	python setup.py sdist upload -r pypi
