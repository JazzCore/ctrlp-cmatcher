@echo off
pushd autoload
python setup.py build -c mingw32
pushd build\lib*
copy fuzzycomt*.pyd ..\..\fuzzycomt.pyd
popd
popd
