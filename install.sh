#!/usr/bin/env sh

cd autoload
python2 setup.py build
cp build/lib*/fuzzycomt.so .
