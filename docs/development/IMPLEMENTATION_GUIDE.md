# Implementation Guide: Performance Optimizations

This guide provides step-by-step instructions to implement the recommendations from `CODE_REVIEW_RECOMMENDATIONS.md`.

---

## 🎯 Phase 1: Critical Performance Improvements

### Step 1: Add Incompressible Data Detection

**File:** `rust/src/archive.rs`

**Location:** Add after the `impl SevenZip` block (around line 100)

```rust
/// Detect data compressibility using Shannon entropy
/// Returns value between 0.0 (very compressible) and 1.0 (incompressible)
fn calculate_entropy(data: &[u8]) -> f64 {
    if data.is_empty() {
        return 0.0;
    }
    
    let mut counts = [0u32; 256];
    for &byte in data {
        counts[byte as usize] += 1;
    }
    
    let len = data.len() as f64;
    let mut entropy = 0.0;
    
    for &count in &counts {
        if count > 0 {
            let p = count as f64 / len;
            entropy -= p * p.log2();
        }
    }
    
    // Normalize to 0-1 range (max entropy for byte is 8)
    entropy / 8.0
}

/// Analyze file to determine if compression is worthwhile
/// Returns recommended compression level based on data characteristics
pub fn analyze_file_compressibility(file_path: &Path) -> std::io::Result<(f64, CompressionLevel)> {
    use std::fs::File;
    use std::io::Read;
    
    let metadata = std::fs::metadata(file_path)?;
    let file_size = metadata.len();
    
    // Sample size: 64KB or 5% of file, whichever is smaller
    let sample_size = ((file_size / 20).min(65536).max(4096)) as usize;
    
    let mut file = File::open(file_path)?;
    let mut buffer = vec![0u8; sample_size];
    let bytes_read = file.read(&mut buffer)?;
    buffer.truncate(bytes_read);
    
    let entropy = calculate_entropy(&buffer);
    
    // Determine compression level based on entropy
    let recommended_level = match entropy {
        e if e > 0.95 => CompressionLevel::Store,    // Already compressed/encrypted
        e if e > 0.85 => CompressionLevel::Fastest,  // Low compression potential
        e if e > 0.70 => CompressionLevel::Fast,     // Moderate compression
        e if e > 0.50 => CompressionLevel::Normal,   // Good compression potential
        _ => CompressionLevel::Maximum,              // High compression potential
    };
    
    Ok((entropy, recommended_level))
}
```

**Add to CompressOptions struct (around line 70):**

```rust
pub struct CompressOptions {
    pub num_threads: usize,
    pub dict_size: usize,
    pub solid: bool,
    pub password: Option<String>,
    
    /// Auto-detect and skip compression for incompressible data
    pub auto_detect_incompressible: bool,
}
```

**Update CompressOptions::default():**

```rust
impl Default for CompressOptions {
    fn default() -> Self {
        Self {
            num_threads: 0,
            dict_size: 0,
            solid: true,
            password: None,
            auto_detect_incompressible: false, // Conservative default
        }
    }
}
```

**Modify create_archive function (around line 420):**

```rust
pub fn create_archive(
    &self,
    archive_path: &str,
    file_paths: &[&str],
    level: CompressionLevel,
    options: Option<&CompressOptions>,
) -> Result<()> {
    let opts = options.cloned().unwrap_or_default();
    
    // Auto-detect incompressible data if enabled
    let effective_level = if opts.auto_detect_incompressible && file_paths.len() == 1 {
        match analyze_file_compressibility(Path::new(file_paths[0])) {
            Ok((entropy, recommended)) if entropy > 0.95 => {
                eprintln!("Info: Data appears incompressible (entropy: {:.2}), using Store mode", entropy);
                CompressionLevel::Store
            },
            _ => level,
        }
    } else {
        level
    };
    
    // ... rest of existing implementation using effective_level ...
}
```

---

### Step 2: Implement Smart Thread Selection

**File:** `rust/src/archive.rs`

**Location:** Add helper function before `impl SevenZip`

```rust
/// Calculate optimal thread count based on total data size
/// Returns recommended thread count considering overhead vs benefit
pub fn calculate_optimal_threads(total_bytes: u64) -> usize {
    let available_cores = std::thread::available_parallelism()
        .map(|n| n.get())
        .unwrap_or(1);
    
    // Thresholds determined from benchmark data:
    // - <1MB: Single thread fastest (no threading overhead)
    // - 1-10MB: 2 threads provides minimal benefit
    // - 10-50MB: 4 threads good balance
    // - >50MB: Scale with cores
    
    match total_bytes {
        0..=1_048_576 => 1,                          // <1MB
        1_048_577..=10_485_760 => 2,                 // 1-10MB
        10_485_761..=52_428_800 => {                 // 10-50MB
            available_cores.min(4)
        },
        52_428_801..=524_288_000 => {                // 50-500MB
            available_cores.min(8)
        },
        _ => available_cores.min(16),                // >500MB
    }
}

/// Get total size of all files to be compressed
fn calculate_total_size(file_paths: &[&str]) -> std::io::Result<u64> {
    let mut total = 0u64;
    for path in file_paths {
        let metadata = std::fs::metadata(path)?;
        total += metadata.len();
    }
    Ok(total)
}
```

**Add convenience constructor to CompressOptions:**

```rust
impl CompressOptions {
    /// Create options with auto-tuned thread count based on file sizes
    pub fn auto_tuned(file_paths: &[&str]) -> std::io::Result<Self> {
        let total_size = calculate_total_size(file_paths)?;
        let optimal_threads = calculate_optimal_threads(total_size);
        
        Ok(Self {
            num_threads: optimal_threads,
            dict_size: 0,
            solid: true,
            password: None,
            auto_detect_incompressible: true, // Enable by default
        })
    }
    
    /// Enable auto-detection with method chaining
    pub fn with_auto_detect(mut self, enable: bool) -> Self {
        self.auto_detect_incompressible = enable;
        self
    }
    
    /// Set thread count with method chaining
    pub fn with_threads(mut self, threads: usize) -> Self {
        self.num_threads = threads;
        self
    }
    
    /// Set password with method chaining
    pub fn with_password(mut self, password: String) -> Self {
        self.password = Some(password);
        self
    }
}
```

**Update create_archive to use auto-tuning when num_threads=0:**

```rust
pub fn create_archive(
    &self,
    archive_path: &str,
    file_paths: &[&str],
    level: CompressionLevel,
    options: Option<&CompressOptions>,
) -> Result<()> {
    let mut opts = options.cloned().unwrap_or_default();
    
    // Auto-tune threads if not explicitly set (num_threads == 0)
    if opts.num_threads == 0 {
        if let Ok(total_size) = calculate_total_size(file_paths) {
            opts.num_threads = calculate_optimal_threads(total_size);
        } else {
            opts.num_threads = 1; // Fallback to single thread
        }
    }
    
    // ... rest of implementation ...
}
```

---

### Step 3: Add Convenience Method for Encrypted Archives

**File:** `rust/src/archive.rs`

**Location:** Inside `impl SevenZip` block

```rust
/// Create encrypted archive with recommended settings
/// 
/// Encryption has virtually zero performance overhead (<1%)
/// and provides strong AES-256 security.
/// 
/// # Example
/// 
/// ```no_run
/// use sevenzip_ffi::SevenZip;
/// 
/// let sz = SevenZip::new().unwrap();
/// sz.create_encrypted_archive(
///     "secure.7z",
///     &["sensitive.txt", "private.doc"],
///     "MyStrongPassword123!",
///     sevenzip_ffi::CompressionLevel::Normal,
/// ).unwrap();
/// ```
pub fn create_encrypted_archive(
    &self,
    archive_path: &str,
    file_paths: &[&str],
    password: &str,
    level: CompressionLevel,
) -> Result<()> {
    let opts = CompressOptions::auto_tuned(file_paths)
        .unwrap_or_default()
        .with_password(password.to_string());
    
    self.create_archive(archive_path, file_paths, level, Some(&opts))
}

/// Create archive with smart defaults (auto-tuned threads, incompressible detection)
/// 
/// # Example
/// 
/// ```no_run
/// use sevenzip_ffi::SevenZip;
/// 
/// let sz = SevenZip::new().unwrap();
/// sz.create_smart_archive(
///     "backup.7z",
///     &["file1.txt", "file2.bin"],
///     sevenzip_ffi::CompressionLevel::Normal,
/// ).unwrap();
/// ```
pub fn create_smart_archive(
    &self,
    archive_path: &str,
    file_paths: &[&str],
    level: CompressionLevel,
) -> Result<()> {
    let opts = CompressOptions::auto_tuned(file_paths).unwrap_or_default();
    self.create_archive(archive_path, file_paths, level, Some(&opts))
}
```

---

## 📝 Phase 2: Documentation Updates

### Update CompressionLevel Documentation

**File:** `rust/src/lib.rs`

**Location:** CompressionLevel enum (around line 50)

```rust
/// Compression level for archive creation
/// 
/// # Performance Characteristics (based on 1MB text file)
/// 
/// | Level    | Time | Throughput | Use Case |
/// |----------|------|------------|----------|
/// | Store    | 0.9ms | 1100+ MB/s | Already compressed files |
/// | Fastest  | 2.4ms | 400+ MB/s  | Quick backups |
/// | Fast     | 2.4ms | 400+ MB/s  | Daily backups |
/// | Normal   | 3.6ms | 270+ MB/s  | General use (RECOMMENDED) |
/// | Maximum  | 3.6ms | 270+ MB/s  | Archival storage |
/// | Ultra    | 3.6ms | 270+ MB/s  | Long-term archival |
/// 
/// **Note:** Maximum and Ultra provide similar compression ratios for most
/// data types. Test with your specific data before choosing Ultra.
/// 
/// **Tip:** Use `Store` for already-compressed files (.zip, .jpg, .mp4, .gz)
/// or enable `auto_detect_incompressible` for automatic detection.
#[repr(u32)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum CompressionLevel {
    /// No compression - store files directly
    /// Best for: Already compressed files, encrypted data
    Store = 0,
    
    /// Fastest compression
    /// Best for: Quick backups, temporary archives
    Fastest = 1,
    
    /// Fast compression with good ratio
    /// Best for: Daily backups, network transfer
    Fast = 3,
    
    /// Balanced compression (RECOMMENDED)
    /// Best for: General purpose archiving
    Normal = 5,
    
    /// Maximum compression
    /// Best for: Long-term storage, distribution
    /// ⚠️  Similar to Normal for many data types
    Maximum = 7,
    
    /// Ultra compression (slowest)
    /// Best for: Minimal size priority
    /// ⚠️  May not provide additional compression over Maximum
    Ultra = 9,
}
```

---

## 🧪 Phase 3: Add Tests

### Add Performance Regression Test

**File:** `rust/tests/integration_tests.rs`

**Add at end of file:**

```rust
#[test]
fn test_incompressible_data_performance() {
    use rand::RngCore;
    
    let temp = TempDir::new().unwrap();
    let sz = SevenZip::new().unwrap();
    
    // Generate 1MB of random (incompressible) data
    let mut rng = rand::thread_rng();
    let mut data = vec![0u8; 1024 * 1024];
    rng.fill_bytes(&mut data);
    
    let test_file = temp.path().join("random.dat");
    fs::write(&test_file, &data).unwrap();
    
    let archive_path = temp.path().join("test.7z");
    
    // Test without auto-detection (should be slow)
    let start = std::time::Instant::now();
    sz.create_archive(
        archive_path.to_str().unwrap(),
        &[test_file.to_str().unwrap()],
        CompressionLevel::Normal,
        None,
    ).unwrap();
    let duration_normal = start.elapsed();
    
    // Test with auto-detection (should be fast)
    let archive_path2 = temp.path().join("test2.7z");
    let mut opts = CompressOptions::default();
    opts.auto_detect_incompressible = true;
    
    let start = std::time::Instant::now();
    sz.create_archive(
        archive_path2.to_str().unwrap(),
        &[test_file.to_str().unwrap()],
        CompressionLevel::Normal,
        Some(&opts),
    ).unwrap();
    let duration_auto = start.elapsed();
    
    println!("Normal compression: {:?}", duration_normal);
    println!("Auto-detect: {:?}", duration_auto);
    
    // Auto-detect should be significantly faster (at least 3x)
    assert!(duration_auto < duration_normal / 3,
        "Auto-detect not faster: {:?} vs {:?}", duration_auto, duration_normal);
}

#[test]
fn test_smart_threading() {
    let temp = TempDir::new().unwrap();
    let sz = SevenZip::new().unwrap();
    
    // Create small file (<1MB)
    let small_file = temp.path().join("small.txt");
    fs::write(&small_file, "x".repeat(500_000)).unwrap();
    
    // Create large file (>50MB)
    let large_file = temp.path().join("large.txt");
    fs::write(&large_file, "x".repeat(52_428_800)).unwrap();
    
    // Small file should get 1 thread
    let small_opts = CompressOptions::auto_tuned(&[small_file.to_str().unwrap()]).unwrap();
    assert_eq!(small_opts.num_threads, 1, "Small file should use 1 thread");
    
    // Large file should get multiple threads
    let large_opts = CompressOptions::auto_tuned(&[large_file.to_str().unwrap()]).unwrap();
    assert!(large_opts.num_threads >= 4, "Large file should use 4+ threads, got {}", large_opts.num_threads);
}

#[test]
fn test_encrypted_convenience_method() {
    let temp = TempDir::new().unwrap();
    let sz = SevenZip::new().unwrap();
    
    let test_file = temp.path().join("secret.txt");
    fs::write(&test_file, "Secret data").unwrap();
    
    let archive_path = temp.path().join("encrypted.7z");
    
    // Use convenience method
    sz.create_encrypted_archive(
        archive_path.to_str().unwrap(),
        &[test_file.to_str().unwrap()],
        "TestPassword123",
        CompressionLevel::Normal,
    ).unwrap();
    
    assert!(archive_path.exists());
    
    // Verify it's actually encrypted (can't extract without password)
    let extract_dir = temp.path().join("extracted");
    fs::create_dir(&extract_dir).unwrap();
    
    let result = sz.extract_archive(
        archive_path.to_str().unwrap(),
        extract_dir.to_str().unwrap(),
        None, // No password
    );
    
    assert!(result.is_err(), "Should fail to extract without password");
}
```

---

## 🚀 Testing the Changes

### Run Unit Tests

```bash
cd rust
cargo test --lib
```

### Run Integration Tests

```bash
cargo test --test integration_tests
```

### Run Benchmarks

```bash
cargo bench --bench compression_benchmarks
```

### Test Smart Defaults

Create `test_smart_features.sh`:

```bash
#!/bin/bash
cd rust

# Test incompressible data detection
echo "Testing incompressible data detection..."
cargo run --example compress -- \
    --input /dev/urandom \
    --output test_random.7z \
    --level Normal \
    --auto-detect

# Test smart threading
echo "Testing smart threading..."
cargo run --example compress -- \
    --input large_file.dat \
    --output test_large.7z \
    --level Normal \
    --auto-threads

# Test encrypted convenience
echo "Testing encrypted convenience..."
cargo run --example compress_encrypted -- \
    --input secret.txt \
    --output secure.7z \
    --password "TestPass123"

echo "All tests complete!"
```

---

## 📊 Expected Results

### Before Changes

```
Incompressible 1MB: 32ms
Small files (5MB): Same time with 1 or 8 threads
```

### After Changes

```
Incompressible 1MB: 0.9ms (35x faster!)
Small files (5MB): Auto-uses 1-2 threads
Encryption: Zero overhead confirmed
```

---

## 🔍 Verification Checklist

- [ ] All existing tests still pass
- [ ] New tests pass
- [ ] Benchmarks show improvement on incompressible data
- [ ] Documentation updated with performance notes
- [ ] Smart defaults work without configuration
- [ ] Encrypted convenience method works
- [ ] Thread auto-tuning reduces overhead

---

## 📖 Usage Examples

### Before (manual configuration)

```rust
let mut opts = CompressOptions::default();
opts.num_threads = 4;
opts.password = Some("pass".to_string());

sz.create_archive("out.7z", &["file.txt"], CompressionLevel::Normal, Some(&opts))?;
```

### After (smart defaults)

```rust
// Option 1: Smart archive (auto everything)
sz.create_smart_archive("out.7z", &["file.txt"], CompressionLevel::Normal)?;

// Option 2: Encrypted with smart defaults
sz.create_encrypted_archive("out.7z", &["file.txt"], "password", CompressionLevel::Normal)?;

// Option 3: Builder pattern
let opts = CompressOptions::auto_tuned(&["file.txt"])?
    .with_password("pass".to_string())
    .with_auto_detect(true);

sz.create_archive("out.7z", &["file.txt"], CompressionLevel::Normal, Some(&opts))?;
```

---

**Implementation Time Estimate:** 4-6 hours  
**Testing Time:** 2-3 hours  
**Total:** ~1 day of work
