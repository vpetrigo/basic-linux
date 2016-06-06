#!/bin/bash

print_zeroes()
{
    for i in `seq 1 $1`; do
        echo 0
    done
}

if [[ $# -eq 1 ]]; then
    print_zeroes $1
fi

