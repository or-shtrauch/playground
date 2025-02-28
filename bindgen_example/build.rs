extern crate bindgen;
use std::env;
use std::path::PathBuf;
use std::process::Command;

fn main() {
    let cool_path = "cool"; // Directory containing Makefile and libcool.a

    // Run `make` to build libcool.a
    let status = Command::new("make")
        .current_dir(cool_path)
        .status()
        .expect("Failed to run make");

    if !status.success() {
        panic!("Makefile failed to build libcool.a");
    }

    // Tell Rust where to find and link the compiled static library
    println!("cargo:rustc-link-search=native={}/", cool_path);
    println!("cargo:rustc-link-lib=static=cool");

    // Generate Rust bindings for C headers
    let bindings = bindgen::Builder::default()
        .header("cool/include/cool.h") // Main header file
        .clang_arg("-Icool/include") // Include path for headers
        .generate()
        .expect("Unable to generate bindings");

    // Write the bindings to the OUT_DIR
    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");
}
