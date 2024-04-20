#!/bin/sh
# Unlike `alltest.sh` running all tests, this script is used to test the program with one specified test copied from `alltest.sh` at one time.
# Usage: specify the task id, e.g., `./quicktest.sh 1`, which means running the test case under the first branch below.
# We have provided 3 test examples. You can add more tests by following the pattern below.

set -e
if [ $# -lt 1 ]; then
    echo "Usage: specify the task id, e.g., ./quicktest.sh 1"
    exit 1
fi

cargo build --no-default-features

if [ "$1" = "1" ]; then
    ./awk -F '-*' '{print $1 "-" $2 "=" $3 "*" $4}'
    # example
    # input: foo--bar
    # expected output: foo-bar=*
elif [ "$1" = "2" ]; then
    ./awk '{ printf "%f %f\n", "000.123", "009.123" }'
    # example
    # expected output: 0.123000 9.123000\n
else 
    ./awk -F '[#]' '{ print NF }'
    # example
    # input: #
    # expected output: 1
    # input: #abc#
    # expected output: 3
fi
