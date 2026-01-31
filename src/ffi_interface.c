#include "7z_ffi.h"
#include "7zCrc.h"  // Add CRC header for CrcGenerateTable()
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define SEVENZIP_VERSION "1.0.0"

/* Global initialization flag */
static int g_initialized = 0;

SevenZipErrorCode sevenzip_init(void) {
    if (g_initialized) {
        return SEVENZIP_OK;
    }
    
    /* Initialize CRC table - CRITICAL: Must be called before any compression operations */
    CrcGenerateTable();
    
    /* Initialize any other global state here */
    
    g_initialized = 1;
    return SEVENZIP_OK;
}

void sevenzip_cleanup(void) {
    if (!g_initialized) {
        return;
    }
    
    /* Cleanup any global state here */
    
    g_initialized = 0;
}

const char* sevenzip_get_error_message(SevenZipErrorCode error_code) {
    switch (error_code) {
        case SEVENZIP_OK:
            return "Success";
        case SEVENZIP_ERROR_OPEN_FILE:
            return "Failed to open file";
        case SEVENZIP_ERROR_INVALID_ARCHIVE:
            return "Invalid or corrupted archive";
        case SEVENZIP_ERROR_MEMORY:
            return "Memory allocation failed";
        case SEVENZIP_ERROR_EXTRACT:
            return "Extraction failed";
        case SEVENZIP_ERROR_COMPRESS:
            return "Compression failed";
        case SEVENZIP_ERROR_INVALID_PARAM:
            return "Invalid parameter";
        case SEVENZIP_ERROR_NOT_IMPLEMENTED:
            return "Feature not implemented";
        default:
            return "Unknown error";
    }
}

const char* sevenzip_get_version(void) {
    return SEVENZIP_VERSION;
}

void sevenzip_free_list(SevenZipList* list) {
    if (!list) {
        return;
    }
    
    if (list->entries) {
        for (size_t i = 0; i < list->count; i++) {
            if (list->entries[i].name) {
                free(list->entries[i].name);
            }
        }
        free(list->entries);
    }
    
    free(list);
}

/* Stub implementation that redirects to sevenzip_create_7z */
SevenZipErrorCode sevenzip_compress(
    const char* archive_path,
    const char** input_paths,
    SevenZipCompressionLevel level,
    const char* password,
    SevenZipProgressCallback progress_callback,
    void* user_data
) {
    SevenZipCompressOptions options = {0};
    options.num_threads = 0;  /* auto */
    options.dict_size = 0;    /* auto */
    options.solid = 1;        /* solid archive */
    options.password = password;
    
    return sevenzip_create_7z(
        archive_path,
        input_paths,
        level,
        &options,
        progress_callback,
        user_data
    );
}
