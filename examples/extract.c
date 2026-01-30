#include "7z_ffi.h"
#include <stdio.h>
#include <stdlib.h>

void progress_callback(uint64_t completed, uint64_t total, void* user_data) {
    printf("\rProgress: %llu/%llu files (%.1f%%)", 
           (unsigned long long)completed, 
           (unsigned long long)total,
           (double)completed / total * 100.0);
    fflush(stdout);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s <archive.7z> <output_dir>\n", argv[0]);
        return 1;
    }
    
    const char* archive_path = argv[1];
    const char* output_dir = argv[2];
    
    printf("7z FFI SDK v%s\n", sevenzip_get_version());
    printf("Extracting: %s\n", archive_path);
    printf("Output to: %s\n\n", output_dir);
    
    /* Initialize library */
    SevenZipErrorCode result = sevenzip_init();
    if (result != SEVENZIP_OK) {
        fprintf(stderr, "Failed to initialize: %s\n", sevenzip_get_error_message(result));
        return 1;
    }
    
    /* Extract archive */
    result = sevenzip_extract(archive_path, output_dir, NULL, progress_callback, NULL);
    
    printf("\n");
    
    if (result == SEVENZIP_OK) {
        printf("Extraction completed successfully!\n");
    } else {
        fprintf(stderr, "Extraction failed: %s\n", sevenzip_get_error_message(result));
    }
    
    /* Cleanup */
    sevenzip_cleanup();
    
    return (result == SEVENZIP_OK) ? 0 : 1;
}
