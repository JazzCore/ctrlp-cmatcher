#!/usr/bin/env sh

cd autoload
python setup.py build
cp build/lib*/fuzzycomt*.so fuzzycomt.so
