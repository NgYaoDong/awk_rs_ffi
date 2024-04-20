## Part B: Partial Translation of a Long C Program to Rust

This folder contains the Rust file (`src/main.rs`) to work on and the original C program (`orig_c/`) to translate from.  

### How to Build & Run & Test the Rust Program

```sh
# build the Rust code
cargo build

# run the Rust code
./target/debug/awk_rs

# run all tests, similar to `alltests.sh` for `awk` C binary
./alltests_rs.sh

# run one test, similar to `quicktest.sh` for `awk` C binary
./quicktest_rs.sh 1
```

See [`workspace/long/orig_c/README.md`](./orig_c/README.md) for more details on the original C program and how to use test scripts.

### Notes for Bindings

Bindings for C functions, variables, and data types are provided in `./bindings/bindings.rs`. It should be enough for the translation. If you find that you need additional bindings, please contact with TAs to have a check first. After the check, if you need to add bindings, you can put the C declarations in `./bindings/bindings.h` and run `./bindings.sh` to update the Rust binding file.

### Notes for Dependencies  

If you need 3rd party libraries in addition to the standard library, please check the Clarifications slides to see if they are in the allow-list. If they are allowed, you can simply add them to `Cargo.toml` and use them in your Rust code. Otherwise, please contact with TAs to have a check first.