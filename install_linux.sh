#!/bin/bash
pushd autoload
python2 setup.py build
pushd build/lib*
cp fuzzycomt.so ../../
popd
popd
