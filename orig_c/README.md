# Extracted `awk` from BusyBox for CS3235 Course Project

This folder contains the extracted C code of `awk` applet from BusyBox (historically has a lot of CVEs!) for CS3235 course project. The original Github repository link is [here](https://github.com/mirror/busybox) (version `bcc5b0e`). We have extracted only necessary code for the `awk` applet for simplicity and supported its library version.

## TL;DR

Most of the functions to be translated are in `awk.c` file. Only `main` function is in `individual_bin.c` file. The provided binary `awk` passes all tests when running `./alltests.sh` (some tests are ignored due to different platforms). The provided library `libawk.a` is to be linked by Rust code, which is set up by default. The test results are saved in `awk_c.test.result` and `awk_c.test.result.verbose` files.

If you want to run one specific test on C binary `awk` and check its behavior, you can add this test into `quicktest.sh` file following the existing examples.
Then run `./quicktest.sh <test_number>`. For example, `./quicktest.sh 1` will run the first test case.

To understand the code better / help debug Rust code, you may want to add debug print statements in `awk.c` or add helper functions in `inc_stripped.c` like the existing ones. 
After modifying the C code, remember to rebuild the binary `awk`: first clean the built files by running `make clean` (`make clean_all` will additionally remove the library); then run `make -f Makefile.bin` to build the binary (`make` to build the library). 

Note that running `./alltests.sh` checks if the stdout equals to the expected output. If you have added printing statements into the C code, the tests can fail. Remember to remove / comment them before running `./alltests.sh`. 

## `awk` Applet in BusyBox

`awk` is a domain-specific language designed for text processing and typically used as a data extraction and reporting tool. You can check its test cases in `awk.tests` file to see its functionalities.

BusyBox includes an AWK implementation, which may be used as a standalone applet or as part of the BusyBox suite of tools. This code has some unsuggested C coding practices, such as changing a constant variable, intended integer overflow, etc. However, we keep the code unchanged for the course project.


## Files in this folder

Here is the brief description of the files in this folder:

- `awk`: binary executable for the `awk` applet.
- `libawk.a`: static library for the `awk` applet, which can be linked with Rust programs.
- `libawk.a.nm`: symbol table of the static library `libawk.a`. It contains the exposed functions and global variables in the library.

`awk` and `libawk.a` are built from the same source code files except for one file: `individual_bin.c` is used to build the binary `awk`, and `individual.c` is used to build the static library `libawk.a`. Their difference is that the former has the `main` function, while the latter does not.

Important source code files:

- `awk.c`: **Main focus of the course project.** The main source code file for the functionality.
- `individual_bin.c`: Minimal wrapper to build `awk` binary. Has the `main` function. Used to build the binary `awk`.
- `individual.c`: Same as `individual_bin.c` but without `main` function. Used to build the shared library `libawk.a`.

Other C files:  
- `inc_stripped.h`: Extracted necessary header files for the `awk` binary.
- `inc_stripped.c`: One function converted from macro and some helper functions for printing value of struct members.
- ...


Build scripts:

- `Makefile`: Makefile to build the library `libawk.a`.
- `Makefile.bin`: Makefile to build the binary `awk`.

Test scripts:

- `alltests.sh`: A script to run all the test cases on the binary `awk`. It invokes `awk.tests` script and save brief results and verbose results into `awk_c.test.result` and `awk_c.test.result.verbose` respectively. 
- `awk.tests`: contains the details of all test cases. Some tests may be skipped based on the current platform.
- `quicktest.sh`: A script to test `awk` with one specified test copied from `awk.tests` at one time. 
- `testing.sh`: utility script.


## How to Build & Run & Test

```sh
# build the binary awk
make -f Makefile.bin

# build the static library libawk.a
make -f Makefile
# or
make

# clean up the .o files and the awk binary
make -f Makefile clean
# or
make clean

# clean up the .o files, the awk binary, the libawk.a library
make -f Makefile clean_all
# or
make clean_all

# run the binary. Can run it with arguments like shown in the tests
./awk

# run all tests
./alltests.sh

# run one test, e.g., 
./quicktest.sh 1
# Usage: `./quicktest.sh <test_number>`. For example, `./quicktest.sh 1` will run the first test case.
```



## Modifications you may make to files in this folder

Although C code is for your reference and you don't need and shouldn't modify it, it's possible that slight modifications without the change of the functionality may help you understand the code better or debug the Rust translation. For example, adding some debug print statements or helper functions to print a structure, testing one specific test case, etc. Here are some changes you may consider:

- `awk.c`: adding debug print statements, or calling helpers functions to print the value of a structure, e.g., `print_intvar` in `inc_stripped.c`.
- `inc_stripped.c`: adding helper functions to print the value of a structure, e.g., `print_intvar`.
- `inc_stripped.h`: adding the declaration of the helper functions you added in `inc_stripped.c`.
- `quicktest.sh`: adding existing test cases so that you can run one test at a time.

Please do no modify files in `./project/source` folder but in `./project/workspace/long` folder, which is a copy of the former. This folder will be automatically created during the initialization, which is mentioned in the instruction markdown file. 
