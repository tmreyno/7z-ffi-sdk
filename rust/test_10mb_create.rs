
use seven_zip::{SevenZip, CompressionLevel, StreamOptions};

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let sz = SevenZip::new()?;
    let mut opt = StreamOptions::default();
    opt.split_size = 100 * 1024 * 1024; // 100MB - won't split for 10MB file
    
    sz.create_archive_streaming(
        "test_10mb.7z",
        &["test_10mb.bin"],
        CompressionLevel::Fast,
        Some(&opt),
        None
    )?;
    
    println!("Created test_10mb.7z");
    Ok(())
}
