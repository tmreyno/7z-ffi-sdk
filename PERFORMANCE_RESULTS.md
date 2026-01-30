# Performance Results - Forensic Archiver vs 7-Zip

## Test Dataset
- **Path**: `/Users/terryreynolds/1827-1001 Case With Data /1.Evidence`
- **Size**: 82.3 GB (88,450,180,042 bytes)
- **Files**: 171 files, 68 folders
- **Type**: Forensic evidence (E01 disk images, memory dumps, PST files, documents)

## Hardware
- **System**: M3 MacBook Air
- **CPU**: 10 cores (ARM64)
- **Optimizations**: CRC32, NEON SIMD

## Final Results

### 🏆 Our Implementation - WINNER
- **Time**: 8:52 (532 seconds)
- **Speed**: 119 MB/s
- **CPU**: 825%
- **Archive Size**: 59 GB
- **Compression Ratio**: 72%

### 7-Zip -mx=1 (Fastest Compression)
- **Time**: 21:06 (1,266 seconds)
- **Speed**: 69.9 MB/s
- **CPU**: 791%
- **Archive Size**: 59 GB
- **Compression Ratio**: 72%

### 7-Zip -mx=0 (Store Only, No Compression)
- **Time**: 1:51 (111 seconds)
- **Speed**: ~795 MB/s
- **CPU**: 88%
- **Archive Size**: 82 GB
- **Compression Ratio**: 100%

## Performance Advantage

**🎉 70% FASTER than 7-Zip -mx=1!**
- Our speed: 119 MB/s
- 7-Zip speed: 69.9 MB/s
- Time saved: 12:14 (58% faster completion)
- Same compression ratio (59 GB archives)

## Optimal Settings

### LZMA2 Parameters
```c
lzma_level = 0;                      /* Fastest compression level */
props.lzmaProps.dictSize = 1 << 15;  /* 32KB dictionary */
props.lzmaProps.fb = 5;              /* Fast bytes = 5 (minimum) */
props.lzmaProps.mc = 1;              /* Match cycles = 1 (minimum) */
props.lzmaProps.algo = 0;            /* Fast algorithm */
props.lzmaProps.lc = 0;              /* Literal context bits = 0 */
props.lzmaProps.lp = 0;              /* Literal pos bits = 0 */
props.lzmaProps.pb = 0;              /* Pos bits = 0 */
use_hash_chain = 1;                  /* HC4 hash chain mode */
```

### Threading & Buffers
- **Threads**: 10 (all physical cores)
- **Write Buffer**: 4 MB
- **Read Buffer**: 1 MB with `setvbuf()`
- **Block Threading**: Enabled via `Lzma2Enc_SetDataSize()`
- **Multi-threading**: Enabled via `Lzma2EncProps_Normalize()`

### What DIDN'T Work
❌ **Copy Codec Detection**: Tested but SLOWER (66.6 MB/s)
- Overhead of reading file samples and checking compressibility
- Not worth it when Level 0 compression is already so fast
- Added 78% to compression time (8:52 → 15:55)

## Technical Notes

1. **Why Level 0 is Fast**: 
   - Minimal dictionary (32KB) means less memory operations
   - Minimal search parameters (fb=5, mc=1) means less pattern matching
   - Still achieves 72% compression ratio (same as 7-Zip -mx=1)

2. **Multi-threading Advantage**:
   - Better CPU utilization (825% vs 791%)
   - LZMA2 block-level parallelism properly configured
   - All 10 physical cores actively engaged

3. **Buffer Optimization**:
   - Large write buffers (4MB) reduce syscall overhead
   - Buffered reads with `setvbuf()` improve I/O
   - Memory-mapped I/O for large files (>100MB)

4. **Archive Format**:
   - ✅ Single-file archives: Work perfectly with standard 7-Zip
   - ❌ Multi-volume archives: Format issue (can be fixed later)
   - ✅ Compression ratio: Identical to 7-Zip
   - ✅ Archive validity: All archives verify with `7z t`

## Conclusion

**Achieved goal: Significantly faster than 7-Zip while maintaining identical compression ratio.**

The key insights:
1. Use the absolute minimum LZMA2 settings (level 0, 32KB dict)
2. Maximize parallelism with all physical cores
3. Optimize buffers for streaming I/O
4. Don't add unnecessary optimizations (copy codec detection)
5. Keep it simple - the LZMA SDK is already well-optimized

**Result: 70% speed improvement over 7-Zip -mx=1 with identical compression.**
