#!/bin/bash
pushd autoload
python setup.py build
pushd build/lib*
cp fuzzycomt.so ../../
popd
popd
