from distutils.core import setup, Extension
from distutils.command.sdist import sdist as _sdist
import copy

try:
    from Cython.Build import cythonize
except ImportError:
    use_cython = False
else:
    use_cython = True

OPTIMIZED = '-O2'
UNOPTIMIZED = '-O0'
OPTIMIZATION = OPTIMIZED


extension = Extension(
	"cystatsd.collector.collector",
	sources = [
		"cystatsd/collector/collector.pyx",
		"cystatsd/collector/statsd_proto.cpp",
	],
	language="c++",
	include_dirs=["cystatsd/collector"],
	extra_compile_args=[OPTIMIZATION, "--std=c++11"]
)


if use_cython:
    STATS_EXT = cythonize(extension)
else:
    extension_copy = copy.deepcopy(extension)
    extension_copy.sources = [
		"cystatsd/collector/statsd_proto.cpp",
		"cystatsd/collector/collector.cpp",
	]
    STATS_EXT = [extension_copy]


class sdist(_sdist):
    def run(self):
        # Make sure the compiled Cython files in the distribution are up-to-date
        from Cython.Build import cythonize
        cythonize(extension)
        _sdist.run(self)


setup(
    name='cystatsd',
    version='1.1.0',
    description="A c++/cython statsd encoder.  It doesn't make any network calls; it just handles batch encoding.",
    classifiers=[
        'License :: OSI Approved :: MIT License',
        'Intended Audience :: Developers',
        'Programming Language :: Python :: 3'
    ],
    author='Scott Ivey',
    author_email='scott.ivey@gmail.com',
    license='MIT',
    package_data={
        'cystatsd': ['*.pyx', '*.pxd', '*.hpp', '*.pxd', '*.py', '*.cpp']
    },
    packages=[
        'cystatsd', 'cystatsd.collector'
    ],
    ext_modules=STATS_EXT,
    provides=['cystatsd'],
	cmdclass={
		'sdist': sdist
	},
)
