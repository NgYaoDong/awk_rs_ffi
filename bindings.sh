#!/bin/bash

# stop on error
set -e
export RUST_BACKTRACE=1

printf "====== Binding Generation Start...
"
cd bindings
bindgen bindings.h --no-layout-tests -o bindings.rs 
printf "====== Binding Generation Done
"
