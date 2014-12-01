#! /usr/bin/env bash
chkPython2()
{
    cmd=$1
    ret=$($cmd -V 2>&1)
    if [[ "$ret" == "Python 2."* ]]; then
        return 0
    else
        return 1
    fi
}

findPython2()
{
    cmdlst=("python" "python2" "python27" "python2.7" "python26" "python2.6")
    for cmd in "${cmdlst[@]}"; do
        if chkPython2 $cmd; then
            py=$cmd
            break
        fi
    done

    if [[ $py == "" ]]; then
        echo "cannot find python2 automatically" >&2
        while true; do
            read -p "please input your python2.* command: " cmd
            if chkPython2 "$cmd"; then
                py=$cmd
                break
            fi
            echo "verify [$cmd] with -V failed" >&2
        done
    fi

    echo "$py"
}
