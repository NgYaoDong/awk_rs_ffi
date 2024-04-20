// No need to change this file for the course project
fn main() {
    // Tell cargo to look for shared libraries in the specified directory
    println!("cargo:rustc-link-search=./orig_c");
    // Tell cargo to tell rustc to link the library.
    println!("cargo:rustc-link-lib=awk");
    // Tell cargo to invalidate the built crate whenever the wrapper changes
    println!("cargo:rerun-if-changed=./orig_c/awk.c");
}