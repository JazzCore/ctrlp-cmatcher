#!/usr/bin/env sh
. findPython2.sh
py=$(findPython2)
echo "find python2 -> $py"

cd autoload
$py setup.py build
cp build/lib*/fuzzycomt.so .
