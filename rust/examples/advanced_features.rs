//! Comprehensive example demonstrating all advanced features
//!
//! This example shows:
//! - Split/multi-volume archives
//! - Raw LZMA/LZMA2 compression
//! - Enhanced error reporting
//!
//! Run with: cargo run --example advanced_features

use seven_zip::{advanced, CompressionLevel, SevenZip};
use std::fs;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    println!("=== 7-Zip Advanced Features Demo ===\n");
    
    // Setup test directory
    fs::create_dir_all("demo_data")?;
    fs::write("demo_data/file1.txt", "Hello from file 1!\n".repeat(1000))?;
    fs::write("demo_data/file2.txt", "Hello from file 2!\n".repeat(1000))?;
    fs::write("demo_data/large.bin", vec![0u8; 1_000_000])?; // 1MB file
    
    // 1. Get library version
    println!("1. Library Information");
    println!("   Version: {}", advanced::get_version());
    println!("   Error for code 5: {}\n", advanced::get_error_string(5));
    
    // 2. Create a split archive (multi-volume)
    println!("2. Creating Split Archive (500KB volumes)");
    advanced::create_split_archive(
        "demo_split.7z",
        &["demo_data/"],
        CompressionLevel::Normal,
        500_000, // 500KB volumes
        None,
    )?;
    
    // Check if split files were created
    if std::path::Path::new("demo_split.7z.001").exists() {
        println!("   ✓ Split archive created:");
        let mut i = 1;
        loop {
            let volume = format!("demo_split.7z.{:03}", i);
            if std::path::Path::new(&volume).exists() {
                let size = fs::metadata(&volume)?.len();
                println!("     - {} ({} bytes)", volume, size);
                i += 1;
            } else {
                break;
            }
        }
    }
    println!();
    
    // 3. Extract split archive
    println!("3. Extracting Split Archive");
    fs::create_dir_all("demo_split_extracted")?;
    advanced::extract_split_archive(
        "demo_split.7z.001",
        "demo_split_extracted",
        None,
    )?;
    println!("   ✓ Split archive extracted to demo_split_extracted/\n");
    
    // 4. Raw LZMA compression
    println!("4. Raw LZMA Compression (.lzma file)");
    advanced::compress_lzma(
        "demo_data/file1.txt",
        "demo_file1.lzma",
        CompressionLevel::Maximum,
    )?;
    
    let original_size = fs::metadata("demo_data/file1.txt")?.len();
    let compressed_size = fs::metadata("demo_file1.lzma")?.len();
    let ratio = 100.0 - (compressed_size as f64 / original_size as f64 * 100.0);
    println!("   ✓ Created demo_file1.lzma");
    println!("     Original: {} bytes", original_size);
    println!("     Compressed: {} bytes", compressed_size);
    println!("     Ratio: {:.1}%\n", ratio);
    
    // 5. Decompress LZMA file
    println!("5. Decompressing LZMA file");
    advanced::decompress_lzma(
        "demo_file1.lzma",
        "demo_file1_restored.txt",
    )?;
    
    let restored_size = fs::metadata("demo_file1_restored.txt")?.len();
    println!("   ✓ Decompressed to demo_file1_restored.txt");
    println!("     Size: {} bytes (matches original: {})\n",
        restored_size, restored_size == original_size);
    
    // 6. LZMA2/XZ compression
    println!("6. LZMA2/XZ Compression (.xz file)");
    advanced::compress_lzma2(
        "demo_data/large.bin",
        "demo_large.xz",
        CompressionLevel::Fast,
    )?;
    
    let xz_original = fs::metadata("demo_data/large.bin")?.len();
    let xz_compressed = fs::metadata("demo_large.xz")?.len();
    let xz_ratio = 100.0 - (xz_compressed as f64 / xz_original as f64 * 100.0);
    println!("   ✓ Created demo_large.xz");
    println!("     Original: {} bytes", xz_original);
    println!("     Compressed: {} bytes", xz_compressed);
    println!("     Ratio: {:.1}%\n", xz_ratio);
    
    // 7. Decompress XZ file
    println!("7. Decompressing XZ file");
    advanced::decompress_lzma2(
        "demo_large.xz",
        "demo_large_restored.bin",
    )?;
    println!("   ✓ Decompressed to demo_large_restored.bin\n");
    
    // 8. Demonstrate error reporting with invalid archive
    println!("8. Enhanced Error Reporting");
    println!("   Testing with invalid archive...");
    
    // Create an invalid "archive"
    fs::write("invalid.7z", b"This is not a valid 7z archive")?;
    
    let sz = SevenZip::new()?;
    match sz.extract("invalid.7z", "output") {
        Ok(_) => println!("   Unexpected success!"),
        Err(e) => {
            println!("   ✓ Caught error: {}", e);
            
            // Try to get detailed error info
            match advanced::DetailedError::get_last() {
                Ok(detailed) => {
                    println!("     Detailed error:");
                    println!("       Code: {}", detailed.code);
                    println!("       Message: {}", detailed.message);
                    if !detailed.file_context.is_empty() {
                        println!("       File: {}", detailed.file_context);
                    }
                    if !detailed.suggestion.is_empty() {
                        println!("       Suggestion: {}", detailed.suggestion);
                    }
                }
                Err(_) => println!("     (No detailed error info available)"),
            }
        }
    }
    println!();
    
    // 9. Common split archive sizes
    println!("9. Common Volume Sizes for Split Archives");
    println!("   Floppy:      1,474,560 bytes (1.44 MB)");
    println!("   CD:          734,003,200 bytes (700 MB)");
    println!("   DVD:         4,700,372,992 bytes (4.7 GB)");
    println!("   FAT32 limit: 4,294,967,296 bytes (4 GB)");
    println!("   Blu-ray:     25,025,314,816 bytes (25 GB)\n");
    
    // Cleanup
    println!("10. Cleaning Up Demo Files");
    let _ = fs::remove_dir_all("demo_data");
    let _ = fs::remove_dir_all("demo_split_extracted");
    let _ = fs::remove_file("demo_split.7z");
    let _ = fs::remove_file("demo_split.7z.001");
    let _ = fs::remove_file("demo_split.7z.002");
    let _ = fs::remove_file("demo_split.7z.003");
    let _ = fs::remove_file("demo_file1.lzma");
    let _ = fs::remove_file("demo_file1_restored.txt");
    let _ = fs::remove_file("demo_large.xz");
    let _ = fs::remove_file("demo_large_restored.bin");
    let _ = fs::remove_file("invalid.7z");
    println!("   ✓ Cleanup complete\n");
    
    println!("=== Demo Complete ===");
    println!("\nAll advanced features working correctly!");
    println!("Check the source code in examples/advanced_features.rs for details.");
    
    Ok(())
}
