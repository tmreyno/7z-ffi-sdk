#include "7z_ffi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <archive.7z>\n", argv[0]);
        return 1;
    }
    
    const char* archive_path = argv[1];
    
    printf("7z FFI SDK v%s\n", sevenzip_get_version());
    printf("Listing: %s\n\n", archive_path);
    
    /* Initialize library */
    SevenZipErrorCode result = sevenzip_init();
    if (result != SEVENZIP_OK) {
        fprintf(stderr, "Failed to initialize: %s\n", sevenzip_get_error_message(result));
        return 1;
    }
    
    /* List archive contents */
    SevenZipList* list = NULL;
    result = sevenzip_list(archive_path, NULL, &list);
    
    if (result == SEVENZIP_OK && list) {
        printf("%-50s %12s %12s %s\n", "Name", "Size", "Packed", "Modified");
        printf("--------------------------------------------------------------------------------\n");
        
        uint64_t total_size = 0;
        uint64_t total_packed = 0;
        
        for (size_t i = 0; i < list->count; i++) {
            SevenZipEntry* entry = &list->entries[i];
            
            char time_str[32] = "";
            if (entry->modified_time > 0) {
                time_t t = (time_t)entry->modified_time;
                struct tm* tm_info = localtime(&t);
                strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M", tm_info);
            }
            
            printf("%-50s %12llu %12llu %s%s\n",
                   entry->name ? entry->name : "(unknown)",
                   (unsigned long long)entry->size,
                   (unsigned long long)entry->packed_size,
                   time_str,
                   entry->is_directory ? " [DIR]" : "");
            
            if (!entry->is_directory) {
                total_size += entry->size;
                total_packed += entry->packed_size;
            }
        }
        
        printf("--------------------------------------------------------------------------------\n");
        printf("Total files: %zu\n", list->count);
        printf("Total size: %llu bytes\n", (unsigned long long)total_size);
        
        if (total_packed > 0 && total_size > 0) {
            printf("Packed size: %llu bytes (%.1f%% compression)\n", 
                   (unsigned long long)total_packed,
                   (1.0 - (double)total_packed / total_size) * 100.0);
        }
        
        /* Free list */
        sevenzip_free_list(list);
    } else {
        fprintf(stderr, "Failed to list archive: %s\n", sevenzip_get_error_message(result));
    }
    
    /* Cleanup */
    sevenzip_cleanup();
    
    return (result == SEVENZIP_OK) ? 0 : 1;
}
