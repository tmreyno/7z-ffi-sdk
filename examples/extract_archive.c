/**
 * Example: Extract Multi-file Archive
 * 
 * Demonstrates how to extract archives created with sevenzip_create_archive()
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
    printf("Multi-file Archive Extraction Example\n\n");
    
    if (argc < 3) {
        printf("Usage: %s <archive_file> <output_directory>\n", argv[0]);
        printf("\nExample:\n");
        printf("  %s archive.7zff extracted_files/\n", argv[0]);
        return 1;
    }
    
    const char* archive_path = argv[1];
    const char* output_dir = argv[2];
    
    // Initialize library
    SevenZipErrorCode result = sevenzip_init();
    if (result != SEVENZIP_OK) {
        fprintf(stderr, "Failed to initialize: %s\n", 
                sevenzip_get_error_message(result));
        return 1;
    }
    
    printf("Extracting: %s\n", archive_path);
    printf("Output to: %s\n\n", output_dir);
    
    // Extract archive
    result = sevenzip_extract_archive(
        archive_path,
        output_dir,
        NULL,  // No password
        progress_callback,
        NULL
    );
    
    printf("\n");
    
    if (result == SEVENZIP_OK) {
        printf("✓ Extraction completed successfully!\n");
    } else {
        fprintf(stderr, "✗ Extraction failed: %s\n", 
                sevenzip_get_error_message(result));
    }
    
    // Cleanup
    sevenzip_cleanup();
    
    return (result == SEVENZIP_OK) ? 0 : 1;
}
