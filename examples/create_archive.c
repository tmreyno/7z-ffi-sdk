/**
 * Example: Create Multi-file Archive
 * 
 * Demonstrates how to create an archive with multiple files using LZMA2 compression
 */

#include "7z_ffi.h"
#include <stdio.h>
#include <stdlib.h>

// Progress callback
void progress_callback(uint64_t completed, uint64_t total, void* user_data) {
    if (total > 0) {
        double percent = (completed * 100.0) / total;
        printf("\rProgress: %llu/%llu files (%.1f%%)", 
               (unsigned long long)completed, 
               (unsigned long long)total, 
               percent);
        fflush(stdout);
    }
}

int main(int argc, char* argv[]) {
    printf("7z FFI SDK v%s\n", sevenzip_get_version());
    printf("Multi-file Archive Creation Example\n\n");
    
    if (argc < 3) {
        printf("Usage: %s <output_archive> <file1> [file2] [file3] ...\n", argv[0]);
        printf("\nExample:\n");
        printf("  %s archive.7zff file1.txt file2.txt file3.txt\n", argv[0]);
        printf("\nCompression levels:\n");
        printf("  Set COMPRESSION_LEVEL environment variable (0-9, default: 5)\n");
        printf("  0 = Store (no compression)\n");
        printf("  1 = Fastest\n");
        printf("  3 = Fast\n");
        printf("  5 = Normal (default)\n");
        printf("  7 = Maximum\n");
        printf("  9 = Ultra\n");
        return 1;
    }
    
    const char* archive_path = argv[1];
    const char** input_files = (const char**)&argv[2];
    int num_files = argc - 2;
    
    // Get compression level from environment or use default
    const char* level_env = getenv("COMPRESSION_LEVEL");
    SevenZipCompressionLevel level = SEVENZIP_LEVEL_NORMAL;
    if (level_env) {
        int level_num = atoi(level_env);
        if (level_num >= 0 && level_num <= 9) {
            level = (SevenZipCompressionLevel)level_num;
        }
    }
    
    const char* level_names[] = {
        "Store", "Fastest", "Fastest", "Fast", "Fast",
        "Normal", "Normal", "Maximum", "Maximum", "Ultra"
    };
    
    // Initialize library
    SevenZipErrorCode result = sevenzip_init();
    if (result != SEVENZIP_OK) {
        fprintf(stderr, "Failed to initialize: %s\n", 
                sevenzip_get_error_message(result));
        return 1;
    }
    
    printf("Creating: %s\n", archive_path);
    printf("Input files: %d\n", num_files);
    for (int i = 0; i < num_files; i++) {
        printf("  %s\n", input_files[i]);
    }
    printf("Compression level: %s\n\n", level_names[level]);
    
    // Create archive
    result = sevenzip_create_archive(
        archive_path,
        input_files,
        level,
        NULL,  // No password
        progress_callback,
        NULL
    );
    
    printf("\n");
    
    if (result == SEVENZIP_OK) {
        printf("✓ Archive created successfully!\n");
    } else {
        fprintf(stderr, "✗ Archive creation failed: %s\n", 
                sevenzip_get_error_message(result));
    }
    
    // Cleanup
    sevenzip_cleanup();
    
    return (result == SEVENZIP_OK) ? 0 : 1;
}
