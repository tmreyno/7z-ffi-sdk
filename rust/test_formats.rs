use seven_zip::SevenZip;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let sz = SevenZip::new()?;
    
    // Test with a zip file
    println!("Testing ZIP file support...");
    match sz.list("../test.zip", None) {
        Ok(entries) => {
            println!("✓ ZIP format supported!");
            println!("  Files: {}", entries.len());
        }
        Err(e) => println!("✗ ZIP format: {}", e),
    }
    
    Ok(())
}
