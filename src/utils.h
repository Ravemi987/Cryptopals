#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <float.h>
#include <math.h>

uint8_t* str_to_hex_bytes(uint8_t* str, size_t len);

uint8_t* hex_bytes_to_str(uint8_t *hex, size_t len);

uint8_t* hex_bytes_to_base64(uint8_t *hex, size_t len, size_t *out_len);

uint8_t* base64_to_hex_bytes(uint8_t *b64, size_t len, size_t *out_len);

uint8_t* fixed_xor(uint8_t* buffer1, uint8_t* buffer2, size_t len);

double single_byte_xor_cipher(uint8_t* cipher, uint8_t *key, size_t len);

double detect_single_char_xor(char* filename, char* decrypted, char* key, size_t len);

uint8_t* repeating_key_xor(uint8_t* text, size_t size, uint8_t* key, size_t key_len);

uint8_t* break_repeating_key_xor(uint8_t* cipher, size_t len, size_t *key_len);

#endif
