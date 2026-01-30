use std::env;
use std::path::PathBuf;

fn main() {
    let manifest_dir = PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap());
    let build_dir = manifest_dir.join("build");
    
    // Check if static library exists
    let static_lib = build_dir.join("lib7z_ffi.a");
    let has_static = static_lib.exists();
    
    // Check if we should use static linking (default)
    let use_static = cfg!(feature = "static") || !cfg!(feature = "dynamic");
    
    println!("cargo:rustc-link-search=native={}", build_dir.display());
    
    if use_static && has_static {
        println!("cargo:rustc-link-lib=static=7z_ffi");
        
        // Link C++ standard library for LZMA SDK
        #[cfg(target_os = "macos")]
        {
            println!("cargo:rustc-link-lib=dylib=c++");
            println!("cargo:rustc-link-lib=framework=CoreFoundation");
        }
        
        #[cfg(target_os = "linux")]
        {
            println!("cargo:rustc-link-lib=dylib=stdc++");
            println!("cargo:rustc-link-lib=dylib=pthread");
        }
        
        #[cfg(target_os = "windows")]
        {
            println!("cargo:rustc-link-lib=dylib=user32");
            println!("cargo:rustc-link-lib=dylib=ole32");
        }
    } else {
        // Fall back to dynamic linking
        println!("cargo:rustc-link-lib=dylib=7z_ffi");
    }
    
    // Tell cargo to invalidate the built crate whenever the C library changes
    println!("cargo:rerun-if-changed=src/");
    println!("cargo:rerun-if-changed=include/");
    println!("cargo:rerun-if-changed=CMakeLists.txt");
}
