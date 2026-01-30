#include "7z_ffi.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

void progress_callback(uint64_t completed, uint64_t total, void* user_data) {
    printf("\rProgress: %llu/%llu files (%.1f%%)", 
           (unsigned long long)completed, 
           (unsigned long long)total,
           (double)completed / total * 100.0);
    fflush(stdout);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s <output.lzma> <input_file>\n", argv[0]);
        printf("\nNote: Currently only single file compression to LZMA format is supported.\n");
        printf("      Full 7z archive creation with multiple files is planned for future releases.\n\n");
        printf("Compression levels:\n");
        printf("  0 - Store (no compression)\n");
        printf("  1 - Fastest\n");
        printf("  3 - Fast\n");
        printf("  5 - Normal (default)\n");
        printf("  7 - Maximum\n");
        printf("  9 - Ultra\n");
        return 1;
    }
    
    const char* archive_path = argv[1];
    
    /* Build input paths array */
    const char** input_paths = (const char**)malloc(sizeof(char*) * argc);
    for (int i = 2; i < argc; i++) {
        input_paths[i - 2] = argv[i];
    }
    input_paths[argc - 2] = NULL;
    
    int num_files = argc - 2;
    
    printf("7z FFI SDK v%s\n", sevenzip_get_version());
    printf("Creating: %s\n", archive_path);
    printf("Input files: %d\n", num_files);
    for (int i = 0; i < num_files; i++) {
        struct stat st;
        if (stat(input_paths[i], &st) == 0) {
            printf("  %s (%lld bytes)\n", input_paths[i], (long long)st.st_size);
        } else {
            printf("  %s (not found)\n", input_paths[i]);
        }
    }
    printf("Compression level: Normal\n\n");
    
    /* Initialize library */
    SevenZipErrorCode result = sevenzip_init();
    if (result != SEVENZIP_OK) {
        fprintf(stderr, "Failed to initialize: %s\n", sevenzip_get_error_message(result));
        return 1;
    }
    
    /* Compress files */
    result = sevenzip_compress(
        archive_path, 
        input_paths, 
        SEVENZIP_LEVEL_NORMAL, 
        NULL, 
        progress_callback, 
        NULL
    );
    
    printf("\n");
    
    if (result == SEVENZIP_OK) {
        printf("Compression completed successfully!\n\n");
        
        /* Show output file size */
        struct stat st_out;
        if (stat(archive_path, &st_out) == 0) {
            printf("Output file size: %lld bytes\n", (long long)st_out.st_size);
            
            /* Calculate compression ratio if single file */
            if (num_files == 1) {
                struct stat st_in;
                if (stat(input_paths[0], &st_in) == 0 && st_in.st_size > 0) {
                    double ratio = (1.0 - (double)st_out.st_size / st_in.st_size) * 100.0;
                    printf("Compression ratio: %.1f%%\n", ratio);
                }
            }
        }
    } else if (result == SEVENZIP_ERROR_NOT_IMPLEMENTED) {
        fprintf(stderr, "\nMultiple file compression is not yet implemented.\n");
        fprintf(stderr, "Current version supports:\n");
        fprintf(stderr, "  ✓ Single file compression to LZMA format\n");
        fprintf(stderr, "  ✗ Multiple files (planned for v1.1.0)\n");
        fprintf(stderr, "  ✗ Directory compression (planned for v1.1.0)\n");
        fprintf(stderr, "  ✗ 7z archive format (planned for v1.1.0)\n");
    } else {
        fprintf(stderr, "Compression failed: %s\n", sevenzip_get_error_message(result));
    }
    
    /* Cleanup */
    sevenzip_cleanup();
    free(input_paths);
    
    return (result == SEVENZIP_OK) ? 0 : 1;
}
