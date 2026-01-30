/**
 * Example: Create Standard .7z Archive
 * 
 * Demonstrates how to create .7z archives compatible with official 7-Zip
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
    printf("Create Standard .7z Archive Example\n\n");
    
    if (argc < 3) {
        printf("Usage: %s <output.7z> <file1> [file2] [...]\n", argv[0]);
        printf("\nExample:\n");
        printf("  %s archive.7z file1.txt file2.txt file3.txt\n", argv[0]);
        printf("\nEnvironment Variables:\n");
        printf("  COMPRESSION_LEVEL - Set compression level (0-9, default: 5)\n");
        printf("    0 = Store (no compression)\n");
        printf("    1 = Fastest\n");
        printf("    3 = Fast\n");
        printf("    5 = Normal (default)\n");
        printf("    7 = Maximum\n");
        printf("    9 = Ultra\n");
        return 1;
    }
    
    const char* archive_path = argv[1];
    const char** input_paths = (const char**)&argv[2];
    int file_count = argc - 2;
    
    // Get compression level from environment
    const char* level_env = getenv("COMPRESSION_LEVEL");
    SevenZipCompressionLevel level = SEVENZIP_LEVEL_NORMAL;
    if (level_env) {
        int level_num = atoi(level_env);
        switch (level_num) {
            case 0: level = SEVENZIP_LEVEL_STORE; break;
            case 1: level = SEVENZIP_LEVEL_FASTEST; break;
            case 3: level = SEVENZIP_LEVEL_FAST; break;
            case 5: level = SEVENZIP_LEVEL_NORMAL; break;
            case 7: level = SEVENZIP_LEVEL_MAXIMUM; break;
            case 9: level = SEVENZIP_LEVEL_ULTRA; break;
            default:
                fprintf(stderr, "Warning: Invalid compression level %d, using Normal (5)\n", 
                        level_num);
        }
    }
    
    // Initialize library
    SevenZipErrorCode result = sevenzip_init();
    if (result != SEVENZIP_OK) {
        fprintf(stderr, "Failed to initialize: %s\n", 
                sevenzip_get_error_message(result));
        return 1;
    }
    
    printf("Creating: %s\n", archive_path);
    printf("Input files: %d\n", file_count);
    for (int i = 0; i < file_count; i++) {
        printf("  %s\n", input_paths[i]);
    }
    printf("Compression level: %s\n\n", 
           level == SEVENZIP_LEVEL_STORE ? "Store" :
           level == SEVENZIP_LEVEL_FASTEST ? "Fastest" :
           level == SEVENZIP_LEVEL_FAST ? "Fast" :
           level == SEVENZIP_LEVEL_NORMAL ? "Normal" :
           level == SEVENZIP_LEVEL_MAXIMUM ? "Maximum" :
           level == SEVENZIP_LEVEL_ULTRA ? "Ultra" : "Unknown");
    
    // Create null-terminated array
    const char** paths_null_term = (const char**)malloc((file_count + 1) * sizeof(char*));
    for (int i = 0; i < file_count; i++) {
        paths_null_term[i] = input_paths[i];
    }
    paths_null_term[file_count] = NULL;
    
    // Create archive
    result = sevenzip_create_7z(
        archive_path,
        paths_null_term,
        level,
        NULL,  // No password
        progress_callback,
        NULL
    );
    
    printf("\n");
    
    free(paths_null_term);
    sevenzip_cleanup();
    
    if (result == SEVENZIP_OK) {
        printf("\n✓ Archive created successfully!\n");
        return 0;
    } else {
        fprintf(stderr, "\n✗ Failed to create archive: %s\n", 
                sevenzip_get_error_message(result));
        return 1;
    }
}
