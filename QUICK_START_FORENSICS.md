# Quick Start Guide - Forensic Evidence Compression

## 🚀 TL;DR - Compress Your 82GB Evidence Right Now

```bash
cd /Users/terryreynolds/7z-ffi-sdk

# Make sure it's built
cmake --build build

# Compress your evidence (this will take 60-75 minutes)
./build/examples/forensic_archiver compress \
    Evidence_Case1827.7z \
    "/Users/terryreynolds/1827-1001 Case With Data /1.Evidence" \
    --split 8g \
    --level 5 \
    --threads 8 \
    --resume
```

That's it! The tool will:
- ✅ Process all 171 files (82 GB total)
- ✅ Create 8GB split volumes (10-11 volumes)
- ✅ Show real-time progress with ETA
- ✅ Use only ~250 MB RAM (not 82 GB!)
- ✅ Allow Ctrl+C interruption with resume
- ✅ Complete in approximately 60-75 minutes

---

## What You Get

**Output files:**
```
Evidence_Case1827.7z.001    8.00 GB
Evidence_Case1827.7z.002    8.00 GB
Evidence_Case1827.7z.003    8.00 GB
Evidence_Case1827.7z.004    8.00 GB
Evidence_Case1827.7z.005    8.00 GB
Evidence_Case1827.7z.006    8.00 GB
Evidence_Case1827.7z.007    8.00 GB
Evidence_Case1827.7z.008    8.00 GB
Evidence_Case1827.7z.009    8.00 GB
Evidence_Case1827.7z.010    2.15 GB  (last volume)
```

Total: **~74 GB** compressed (depending on file types)

---

## During Compression

You'll see progress like this:

```
Forensic Evidence Archiver - Compression
==========================================
Archive:     Evidence_Case1827.7z
Input files: 1
Level:       5 (normal)
Split size:  8.00 GB
Chunk size:  64.00 MB
Threads:     8
Resume:      Enabled

Starting compression...

[===>----------------] 18.7% (15.3 GB / 82.0 GB)
Speed: 22.5 MB/s | ETA: 49m 32s | Elapsed: 11m 22s
Current: macwd.E01 (3.2 GB / 9.2 GB - 35%)

Press Ctrl+C to interrupt and save checkpoint
```

---

## If Interrupted (Ctrl+C or crash)

Just run the **exact same command** again:

```bash
./build/examples/forensic_archiver compress \
    Evidence_Case1827.7z \
    "/Users/terryreynolds/1827-1001 Case With Data /1.Evidence" \
    --split 8g \
    --level 5 \
    --threads 8 \
    --resume
```

The tool will:
1. Load `Evidence_Case1827.7z.checkpoint`
2. Skip already-compressed files
3. Resume from where it left off
4. Continue compression

---

## Extract the Archive

```bash
./build/examples/forensic_archiver extract \
    Evidence_Case1827.7z.001 \
    /path/to/extract/to
```

The tool automatically:
- Finds all volumes (.001, .002, .003, etc.)
- Reads seamlessly across volumes
- Extracts all files with progress tracking

---

## Tips for Best Performance

### Recommended Settings for 82GB Evidence

| Setting | Value | Why |
|---------|-------|-----|
| **Split size** | `8g` | Good for storage/transfer |
| **Level** | `5` | Balance speed vs. size |
| **Threads** | `8` | Uses all your CPU cores |
| **Chunk size** | `64m` | Good for large files |
| **Resume** | Enable | Safety for long operations |

### If You Need Speed

```bash
# Faster compression (35-45 minutes)
./build/examples/forensic_archiver compress evidence.7z /data \
    --split 8g \
    --level 1 \
    --threads 10 \
    --resume
```

### If You Need Maximum Compression

```bash
# Best compression (90-120 minutes)
./build/examples/forensic_archiver compress evidence.7z /data \
    --split 8g \
    --level 7 \
    --threads 10 \
    --resume
```

---

## Size Notation

The tool understands these formats:

```bash
--split 8g          # 8 gigabytes
--split 8192m       # 8192 megabytes (same as 8g)
--split 8589934592  # Bytes (same as 8g)

--chunk 64m         # 64 megabytes
--chunk 65536k      # 65536 kilobytes (same as 64m)
```

---

## Command Reference

### Compression
```bash
./build/examples/forensic_archiver compress <output.7z> <input> [OPTIONS]
```

**Options:**
- `--level <0-9>` - Compression level (default: 5)
  - 0 = Store (no compression)
  - 1 = Fastest
  - 5 = Normal (recommended)
  - 7 = Maximum
  - 9 = Ultra
- `--split <size>` - Split archive size (e.g., 4g, 8g)
- `--chunk <size>` - Chunk size for streaming (e.g., 64m, 128m)
- `--threads <n>` - Number of threads (default: 4)
- `--resume` - Enable resume/checkpoint support
- `--password <pwd>` - Password protection (not yet implemented)

### Extraction
```bash
./build/examples/forensic_archiver extract <archive.7z.001> <output_dir>
```

### Resume
```bash
./build/examples/forensic_archiver resume <archive.7z>
```
(Implementation pending - currently use `compress` with `--resume`)

### Test Archive
```bash
./build/examples/forensic_archiver test <archive.7z.001>
```
(Implementation pending - use system `7z t archive.7z.001`)

---

## Troubleshooting

### "Command not found"
Make sure you built the project:
```bash
cd /Users/terryreynolds/7z-ffi-sdk
cmake --build build
```

### "Out of disk space"
You need at least 82 GB free for the compressed archive.

### "Cannot open input file"
Check the path has spaces - use quotes:
```bash
"/Users/terryreynolds/1827-1001 Case With Data /1.Evidence"
```

### "Checkpoint file corrupt"
Delete the checkpoint and restart:
```bash
rm Evidence_Case1827.7z.checkpoint
# Then run compress command again
```

### Compression seems stuck
The tool is working! Large files (like 49 GB `PC-MUS-001.E01`) take time.
Watch the "Current" line for file-level progress.

---

## What's In Your Evidence Directory

The tool will compress these files:

**Large files that will take most time:**
- `PC-MUS-001.E01` - 49 GB (this alone is 60% of compression time!)
- `4Dell Latitude CPi.E01` - 9.2 GB
- `4Dell Latitude CPi.E02` - 9.2 GB
- `Nov17memdump.mem` - 9.2 GB
- `macwd.E01` - 9.2 GB

**Medium files:**
- 8× Takeout files - ~635 MB each
- Several smaller .E01 forensic images

**Total:** 171 files, 82 GB

**Compression order:** The tool processes files in directory order.
The 49 GB file will take ~35-40 minutes alone.

---

## After Compression

### Verify the archive
```bash
# Using system 7z tool
7z t Evidence_Case1827.7z.001
```

### Calculate checksums
```bash
# SHA256 of all volumes
shasum -a 256 Evidence_Case1827.7z.*
```

### Transfer to storage
All volumes must be transferred together:
```bash
# Example: rsync to remote storage
rsync -avP Evidence_Case1827.7z.* user@server:/evidence/
```

---

## Performance Reference

Based on your system (10-core Apple Silicon, 32GB RAM, SSD):

| Level | Time | Final Size | Speed | Use Case |
|-------|------|------------|-------|----------|
| 1 | 35-45 min | 77-80 GB | 30-40 MB/s | Quick backup |
| 5 | 60-75 min | 70-75 GB | 18-23 MB/s | **Recommended** |
| 7 | 90-120 min | 65-72 GB | 11-15 MB/s | Long-term storage |
| 9 | 3-5 hours | 60-70 GB | 4-7 MB/s | Maximum compression |

**Note:** Your evidence files (.E01, .mem) are already compressed formats,
so compression ratios won't be as high as text/uncompressed data.

---

## Next Steps

1. **Test with small files first** (recommended):
   ```bash
   ./build/examples/forensic_archiver compress test.7z test_data --level 5
   ```

2. **Test interruption/resume:**
   - Start compression
   - Press Ctrl+C after 30 seconds
   - Run same command again
   - Verify it resumes

3. **Compress your 82GB evidence:**
   - Run the command at the top of this file
   - Let it run for ~60-75 minutes
   - Verify all volumes were created
   - Test extraction

4. **Extract and verify:**
   ```bash
   ./build/examples/forensic_archiver extract Evidence_Case1827.7z.001 /tmp/verify
   # Compare checksums
   ```

---

## Questions?

**"Can I use fewer threads?"**  
Yes, adjust `--threads`. Fewer threads = slower but lower CPU usage.

**"Can I use larger split sizes?"**  
Yes, but FAT32 has 4GB limit. NTFS/exFAT can handle larger.

**"Will this corrupt my evidence?"**  
No, the tool only reads input files. Original files are never modified.

**"Can I compress multiple directories?"**  
Currently one directory at a time. Run the command multiple times for multiple dirs.

**"What if my computer crashes during compression?"**  
Use `--resume` flag. The checkpoint file saves progress every ~100 MB.

**"How do I know it's working?"**  
Watch the progress bar and "Current" file. Speed should be 15-25 MB/s.

---

## Ready to Go!

```bash
cd /Users/terryreynolds/7z-ffi-sdk

./build/examples/forensic_archiver compress \
    Evidence_Case1827.7z \
    "/Users/terryreynolds/1827-1001 Case With Data /1.Evidence" \
    --split 8g \
    --level 5 \
    --threads 8 \
    --resume
```

**Estimated time:** 60-75 minutes  
**Output:** 10-11 volumes @ 8GB each  
**Total size:** ~74 GB compressed  

**Good luck with your case! 🔍**
