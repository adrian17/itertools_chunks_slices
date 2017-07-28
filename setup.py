from setuptools import setup, Extension

module = Extension(
    'itertools_chunks_slices',
    sources=['itertools_chunks_slices.c'],
)

setup(
    name='itertools_chunks_slices',
    version='0.1',
    description='itertools: chunks() and slices()',
    ext_modules=[module],
    test_suite="tests",
    url="https://github.com/adrian17/",
    author="Adrian Wielgosik",
    author_email="adrian.wielgosik@gmail.com",
    classifiers=[
        "Programming Language :: C",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3.5",
        "Programming Language :: Python :: 3.6",
    ]
)