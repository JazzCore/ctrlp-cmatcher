@echo off
pushd autoload
python setup.py build -c mingw32
pushd build\lib*
xcopy fuzzycomt.pyd ..\..\
popd
popd