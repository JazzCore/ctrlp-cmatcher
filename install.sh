#!/usr/bin/env sh

chkPython2()
{
    cmd=$1
    ret=$($cmd -V 2>&1)
    case "$ret" in
    "Python 2."*)
        return 0
        ;;
    *)
        return 1
        ;;
    esac
}

findPython2()
{
    cmd_list="python python2 python27 python2.7 python26 python2.6"
    for cmd in $cmd_list; do
        if chkPython2 $cmd; then
            found_python=$cmd
            break
        fi
    done

    if [ "$found_python" = "" ]; then
        echo "cannot find python2 automatically" >&2
        while true; do
            read -p "please input your python 2 command: " cmd
            if chkPython2 "$cmd"; then
                found_python=$cmd
                break
            fi
            echo "verify [$cmd] with -V failed" >&2
        done
    fi

    echo $found_python
}

python=$(findPython2)
echo "find python2 -> $python"

cd autoload
$python setup.py build
cp build/lib*/fuzzycomt.so .
