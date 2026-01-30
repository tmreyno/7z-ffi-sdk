/**
 * Example: Decompress LZMA file
 * 
 * Demonstrates how to decompress a standalone .lzma file
 */

#include "7z_ffi.h"
#include <stdio.h>
#include <stdlib.h>

// Progress callback
void progress_callback(uint64_t completed, uint64_t total, void* user_data) {
    if (total > 0) {
        double percent = (completed * 100.0) / total;
        printf("\rProgress: %.1f%% (%llu/%llu bytes)", percent, 
               (unsigned long long)completed, (unsigned long long)total);
        fflush(stdout);
    }
}

int main(int argc, char* argv[]) {
    printf("7z FFI SDK v%s\n", sevenzip_get_version());
    printf("LZMA Decompression Example\n\n");
    
    if (argc < 3) {
        printf("Usage: %s <input.lzma> <output_file>\n", argv[0]);
        printf("\nExample:\n");
        printf("  %s compressed.lzma decompressed.txt\n", argv[0]);
        return 1;
    }
    
    const char* input_path = argv[1];
    const char* output_path = argv[2];
    
    // Initialize library
    SevenZipErrorCode result = sevenzip_init();
    if (result != SEVENZIP_OK) {
        fprintf(stderr, "Failed to initialize: %s\n", 
                sevenzip_get_error_message(result));
        return 1;
    }
    
    printf("Decompressing: %s\n", input_path);
    printf("Output to:     %s\n\n", output_path);
    
    // Decompress file
    result = sevenzip_decompress_lzma(
        input_path,
        output_path,
        progress_callback,
        NULL
    );
    
    printf("\n");
    
    if (result == SEVENZIP_OK) {
        printf("✓ Decompression completed successfully!\n");
    } else {
        fprintf(stderr, "✗ Decompression failed: %s\n", 
                sevenzip_get_error_message(result));
    }
    
    // Cleanup
    sevenzip_cleanup();
    
    return (result == SEVENZIP_OK) ? 0 : 1;
}
