#!/bin/sh

set -e
if [ $# -lt 1 ]; then
    echo "Usage: specify the task id, e.g., 1"
    exit 1
fi

cargo build --no-default-features

if [ "$1" = "1" ]; then
    ./target/debug/awk_rs -F '-*' '{print $1 "-" $2 "=" $3 "*" $4}'
    # example
    # input: foo--bar
    # expected output: foo-bar=*
elif [ "$1" = "2" ]; then
    ./target/debug/awk_rs '{ printf "%f %f\n", "000.123", "009.123" }'
    # example
    # expected output: 0.123000 9.123000\n
else 
    ./target/debug/awk_rs -F '[#]' '{ print NF }'
    # example
    # input: #
    # expected output: 1
    # input: #abc#
    # expected output: 3
fi
