/**
 * Test AES-256 Encryption Implementation
 * 
 * Tests the encryption/decryption functions to verify they work correctly
 */

#include "../include/7z_ffi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Test data
static const char* test_data = "This is a test message for AES-256 encryption! It contains enough data to span multiple blocks.";
static const char* test_password = "TestPassword123!";

// We need to store salt/IV for decryption
static uint8_t g_salt[16];
static uint8_t g_iv[16];

int main() {
    printf("AES-256 Encryption Test\n");
    printf("=======================\n\n");
    
    // Initialize library
    SevenZipErrorCode result = sevenzip_init();
    if (result != SEVENZIP_OK) {
        fprintf(stderr, "Failed to initialize: %s\n", sevenzip_get_error_message(result));
        return 1;
    }
    
    // Seed random for reproducible test
    srand(12345);
    
    printf("Test 1: Key Derivation\n");
    printf("-----------------------\n");
    
    uint8_t key[32];
    uint32_t aes_ctx[AES_NUM_IVMRK_WORDS] __attribute__((aligned(16)));
    
    result = sevenzip_init_encryption(test_password, key, g_iv, aes_ctx);
    if (result != SEVENZIP_OK) {
        fprintf(stderr, "✗ Key derivation failed: %s\n", sevenzip_get_error_message(result));
        return 1;
    }
    
    // Save the salt that was generated (in real code, sevenzip_init_encryption would return it)
    // For now, we'll regenerate it with same seed
    srand(12345);
    for (int i = 0; i < 16; i++) {
        g_salt[i] = (uint8_t)(rand() & 0xFF);
    }
    
    printf("✓ Key derivation successful\n");
    printf("  Key (first 8 bytes): ");
    for (int i = 0; i < 8; i++) {
        printf("%02x ", key[i]);
    }
    printf("\n");
    printf("  IV (first 8 bytes):  ");
    for (int i = 0; i < 8; i++) {
        printf("%02x ", g_iv[i]);
    }
    printf("\n");
    printf("  Salt (first 8 bytes): ");
    for (int i = 0; i < 8; i++) {
        printf("%02x ", g_salt[i]);
    }
    printf("\n\n");
    
    printf("Test 2: Encryption\n");
    printf("------------------\n");
    
    size_t plaintext_len = strlen(test_data);
    printf("Plaintext: \"%s\"\n", test_data);
    printf("Length: %zu bytes\n", plaintext_len);
    
    // Allocate aligned buffer for encryption
    size_t max_encrypted_len = ((plaintext_len / 16) + 2) * 16; // Extra block for padding
    uint8_t* encrypted = (uint8_t*)aligned_alloc(16, max_encrypted_len);
    if (!encrypted) {
        fprintf(stderr, "✗ Memory allocation failed\n");
        return 1;
    }
    
    size_t encrypted_len = max_encrypted_len;
    result = sevenzip_encrypt_data(
        aes_ctx, g_iv,
        (const uint8_t*)test_data, plaintext_len,
        encrypted, &encrypted_len
    );
    
    if (result != SEVENZIP_OK) {
        fprintf(stderr, "✗ Encryption failed: %s\n", sevenzip_get_error_message(result));
        free(encrypted);
        return 1;
    }
    
    printf("✓ Encryption successful\n");
    printf("  Encrypted length: %zu bytes\n", encrypted_len);
    printf("  Ciphertext (first 32 bytes): ");
    for (size_t i = 0; i < 32 && i < encrypted_len; i++) {
        printf("%02x ", encrypted[i]);
    }
    printf("\n\n");
    
    printf("Test 3: Decryption\n");
    printf("------------------\n");
    
    // Initialize decryption using the salt we saved
    uint8_t dec_key[32];
    uint32_t dec_ctx[AES_NUM_IVMRK_WORDS] __attribute__((aligned(16)));
    
    result = sevenzip_init_decryption(test_password, g_salt, 16, dec_key, dec_ctx);
    if (result != SEVENZIP_OK) {
        fprintf(stderr, "✗ Decryption init failed: %s\n", sevenzip_get_error_message(result));
        free(encrypted);
        return 1;
    }
    
    // Decrypt
    uint8_t* decrypted = (uint8_t*)aligned_alloc(16, encrypted_len);
    if (!decrypted) {
        fprintf(stderr, "✗ Memory allocation failed\n");
        free(encrypted);
        return 1;
    }
    
    size_t decrypted_len = encrypted_len;
    result = sevenzip_decrypt_data(
        dec_ctx, g_iv,
        encrypted, encrypted_len,
        decrypted, &decrypted_len
    );
    
    if (result != SEVENZIP_OK) {
        fprintf(stderr, "✗ Decryption failed: %s\n", sevenzip_get_error_message(result));
        free(encrypted);
        free(decrypted);
        return 1;
    }
    
    printf("✓ Decryption successful\n");
    printf("  Decrypted length: %zu bytes\n", decrypted_len);
    printf("  Decrypted text: \"%.*s\"\n", (int)decrypted_len, decrypted);
    
    // Verify correctness
    if (decrypted_len == plaintext_len && memcmp(decrypted, test_data, plaintext_len) == 0) {
        printf("\n✓ PASS: Decrypted data matches original!\n");
    } else {
        printf("\n✗ FAIL: Decrypted data does not match!\n");
        printf("  Expected length: %zu, got: %zu\n", plaintext_len, decrypted_len);
        free(encrypted);
        free(decrypted);
        return 1;
    }
    
    free(encrypted);
    free(decrypted);
    
    printf("\n");
    printf("Test 4: Wrong Password Detection\n");
    printf("---------------------------------\n");
    
    // Try decrypting with wrong password
    const char* wrong_password = "WrongPassword456!";
    
    // Reset rand for consistent test
    srand(12345);
    result = sevenzip_init_encryption(test_password, key, g_iv, aes_ctx);
    encrypted = (uint8_t*)aligned_alloc(16, max_encrypted_len);
    encrypted_len = max_encrypted_len;
    result = sevenzip_encrypt_data(aes_ctx, g_iv, (const uint8_t*)test_data, plaintext_len, encrypted, &encrypted_len);
    
    // Regenerate salt with same seed
    srand(12345);
    for (int i = 0; i < 16; i++) {
        g_salt[i] = (uint8_t)(rand() & 0xFF);
    }
    
    // Try to decrypt with wrong password
    result = sevenzip_init_decryption(wrong_password, g_salt, 16, dec_key, dec_ctx);
    decrypted = (uint8_t*)aligned_alloc(16, encrypted_len);
    decrypted_len = encrypted_len;
    result = sevenzip_decrypt_data(dec_ctx, g_iv, encrypted, encrypted_len, decrypted, &decrypted_len);
    
    if (result == SEVENZIP_OK) {
        // Check if decrypted data makes sense
        int looks_valid = 1;
        for (size_t i = 0; i < decrypted_len && i < 10; i++) {
            if (decrypted[i] == 0 || decrypted[i] > 127) {
                looks_valid = 0;
                break;
            }
        }
        
        if (!looks_valid || decrypted_len != plaintext_len || memcmp(decrypted, test_data, plaintext_len) != 0) {
            printf("✓ Wrong password produces garbage (as expected)\n");
        } else {
            printf("✗ FAIL: Wrong password decrypted correctly (shouldn't happen!)\n");
            free(encrypted);
            free(decrypted);
            return 1;
        }
    } else {
        printf("✓ Wrong password detected via padding error\n");
    }
    
    free(encrypted);
    free(decrypted);
    
    printf("\n");
    printf("======================\n");
    printf("All encryption tests passed!\n");
    printf("======================\n");
    
    sevenzip_cleanup();
    return 0;
}
