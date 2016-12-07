from Cython.Build import cythonize
from distutils.core import setup, Extension

OPTIMIZED = '-O2'
UNOPTIMIZED = '-O0'
OPTIMIZATION = OPTIMIZED

STATS_EXT = cythonize(Extension("cystatsd.collector.collector", sources=[
        "cystatsd/collector/collector.pyx",
        "cystatsd/collector/statsd_proto.cpp"
    ],
    language="c++",
    include_dirs=["cystatsd/collector"],
    extra_compile_args=[OPTIMIZATION, "--std=c++11"]
))

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
        'cystatsd': ['*.pyx', '*.pxd', '*.hpp', '*.pxd', '*.py']
    },
    packages=[
        'cystatsd', 'cystatsd.collector'
    ],
    ext_modules=STATS_EXT,
    provides=['cystatsd']
)
