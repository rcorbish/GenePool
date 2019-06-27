#!/usr/bin/env python

from distutils.core import setup, Extension


chromosone_module = Extension(
    'gene_pool',
    sources=['chromosone.cpp'],
    extra_compile_args = ['--std=c++17', '-g3', '-O0' ],
    libraries = ['stdc++'],
    language='C++17', )

setup(
    name='gene_pool',
    version='0.1.0',
    description='example module written in C++',
    ext_modules=[chromosone_module] )
