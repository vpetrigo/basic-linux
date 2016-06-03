#!/bin/bash

print_zeroes()
{
    for i in `seq 1 $1`; do
        echo 0
    done
}

echo $#
echo $1

if [[ $# -eq 1 ]]; then
    print_zeroes $1
fi

