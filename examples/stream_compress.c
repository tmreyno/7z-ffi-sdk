/**
 * Example: Streaming 7z Archive Creation with Split Volumes
 * 
 * Demonstrates how to compress large files using chunk-based streaming
 * and create split archives for easier transfer.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/7z_ffi.h"

/* Format bytes to human-readable string */
static void format_bytes(uint64_t bytes, char* buffer, size_t size) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit_index = 0;
    double value = (double)bytes;
    
    while (value >= 1024.0 && unit_index < 4) {
        value /= 1024.0;
        unit_index++;
    }
    
    snprintf(buffer, size, "%.2f %s", value, units[unit_index]);
}

/* Progress callback with detailed information */
static void streaming_progress_callback(
    uint64_t bytes_processed,
    uint64_t bytes_total,
    uint64_t current_file_bytes,
    uint64_t current_file_total,
    const char* current_file_name,
    void* user_data
) {
    char processed_str[32], total_str[32], file_processed_str[32], file_total_str[32];
    
    format_bytes(bytes_processed, processed_str, sizeof(processed_str));
    format_bytes(bytes_total, total_str, sizeof(total_str));
    format_bytes(current_file_bytes, file_processed_str, sizeof(file_processed_str));
    format_bytes(current_file_total, file_total_str, sizeof(file_total_str));
    
    double total_percent = bytes_total > 0 ? (100.0 * bytes_processed / bytes_total) : 0.0;
    double file_percent = current_file_total > 0 ? (100.0 * current_file_bytes / current_file_total) : 0.0;
    
    printf("\r[%.1f%%] Overall: %s / %s | File: %s / %s (%.1f%%) - %s",
           total_percent, processed_str, total_str,
           file_processed_str, file_total_str, file_percent,
           current_file_name);
    fflush(stdout);
}

static void print_usage(const char* program) {
    printf("Usage: %s <archive.7z> <input_files...> [options]\n", program);
    printf("\nOptions:\n");
    printf("  --level <0-9>        Compression level (default: 5)\n");
    printf("                       0=store, 1=fastest, 5=normal, 9=ultra\n");
    printf("  --split <size>       Split archive size in bytes\n");
    printf("                       Examples: 4294967296 (4GB), 8589934592 (8GB)\n");
    printf("  --chunk <size>       Chunk size for streaming (default: 67108864 = 64MB)\n");
    printf("  --threads <num>      Number of threads (default: 2, 0=auto)\n");
    printf("  --temp <dir>         Temporary directory\n");
    printf("\nExamples:\n");
    printf("  # Compress large file without split:\n");
    printf("  %s output.7z /path/to/large/file.img\n", program);
    printf("\n");
    printf("  # Compress with 4GB split volumes:\n");
    printf("  %s output.7z /path/to/files --split 4294967296\n", program);
    printf("\n");
    printf("  # Compress entire directory with 8 threads and 8GB splits:\n");
    printf("  %s output.7z /evidence/dir --split 8589934592 --threads 8\n", program);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }
    
    const char* archive_path = argv[1];
    
    /* Parse arguments */
    SevenZipStreamOptions opts;
    sevenzip_stream_options_init(&opts);
    
    SevenZipCompressionLevel level = SEVENZIP_LEVEL_NORMAL;
    
    /* Collect input files and parse options */
    const char** input_files = (const char**)malloc(argc * sizeof(char*));
    int file_count = 0;
    
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--level") == 0 && i + 1 < argc) {
            level = atoi(argv[++i]);
            if (level < 0) level = 0;
            if (level > 9) level = 9;
        } else if (strcmp(argv[i], "--split") == 0 && i + 1 < argc) {
            opts.split_size = strtoull(argv[++i], NULL, 10);
        } else if (strcmp(argv[i], "--chunk") == 0 && i + 1 < argc) {
            opts.chunk_size = strtoull(argv[++i], NULL, 10);
        } else if (strcmp(argv[i], "--threads") == 0 && i + 1 < argc) {
            opts.num_threads = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--temp") == 0 && i + 1 < argc) {
            opts.temp_dir = argv[++i];
        } else {
            input_files[file_count++] = argv[i];
        }
    }
    input_files[file_count] = NULL;
    
    if (file_count == 0) {
        fprintf(stderr, "Error: No input files specified\n");
        free(input_files);
        return 1;
    }
    
    /* Display configuration */
    printf("7z Streaming Compression\n");
    printf("========================\n");
    printf("Archive: %s\n", archive_path);
    printf("Input files: %d\n", file_count);
    printf("Compression level: %d\n", level);
    printf("Threads: %d\n", opts.num_threads);
    
    char split_str[32], chunk_str[32];
    if (opts.split_size > 0) {
        format_bytes(opts.split_size, split_str, sizeof(split_str));
        printf("Split size: %s\n", split_str);
    } else {
        printf("Split: No (single archive)\n");
    }
    
    format_bytes(opts.chunk_size, chunk_str, sizeof(chunk_str));
    printf("Chunk size: %s\n", chunk_str);
    printf("\n");
    
    /* Initialize library */
    SevenZipErrorCode result = sevenzip_init();
    if (result != SEVENZIP_OK) {
        fprintf(stderr, "Failed to initialize library: %s\n", 
                sevenzip_get_error_message(result));
        free(input_files);
        return 1;
    }
    
    /* Create archive with streaming */
    printf("Compressing...\n");
    result = sevenzip_create_7z_streaming(
        archive_path,
        input_files,
        level,
        &opts,
        streaming_progress_callback,
        NULL
    );
    
    printf("\n");
    
    if (result == SEVENZIP_OK) {
        printf("✓ Compression completed successfully!\n");
        
        if (opts.split_size > 0) {
            printf("\nSplit archive created. Files:\n");
            printf("  %s.001\n", archive_path);
            printf("  %s.002\n", archive_path);
            printf("  ...\n");
            printf("\nTo extract, use: 7z x %s.001\n", archive_path);
        }
    } else {
        fprintf(stderr, "✗ Compression failed: %s\n", 
                sevenzip_get_error_message(result));
    }
    
    /* Cleanup */
    sevenzip_cleanup();
    free(input_files);
    
    return result == SEVENZIP_OK ? 0 : 1;
}
