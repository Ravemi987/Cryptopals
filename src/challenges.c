#include "challenges.h"
#include "utils.h"
#include <ctype.h>

void clean_spaces(char *buffer, size_t size, size_t *len) {
    size_t clean_len = 0;
    for (size_t i = 0; i < size; i++) {
        if (!isspace((unsigned char)buffer[i])) {
            buffer[clean_len++] = buffer[i];
        }
    }
    buffer[clean_len] = '\0';
    *len = clean_len;
}

void challenge1(void) {
    char input[] = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";
    char expected[] = "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t";
    size_t b64_in_len;
    size_t b64_out_len;

    uint8_t *hex_bytes = str_to_hex_bytes((uint8_t *)input, strlen(input));
    uint8_t *str = hex_bytes_to_str(hex_bytes, strlen(input) / 2);
    uint8_t *b64_encode = hex_bytes_to_base64(hex_bytes, strlen(input) / 2, &b64_in_len);
    uint8_t *b64_decode = base64_to_hex_bytes(b64_encode, b64_in_len, &b64_out_len);

    printf("Text to hex : %s\n", strcmp(input, (char*) str) == 0 ? "Ok" : "Error");
    printf("Text to base64 : %s\n", strcmp(expected, (char *)b64_encode) == 0 ? "Ok" : "Error");
    
    bool ok = (b64_out_len == strlen(input) / 2) && (memcmp(b64_decode, hex_bytes, b64_out_len) == 0);
    printf("base64 to bytes : %s\n", ok ? "Ok" : "Error");

    free(b64_encode);
    free(b64_decode);
    free(hex_bytes);
    free(str);
}

void challenge2(void) {
    char input1[] = "1c0111001f010100061a024b53535009181c";
    char input2[] = "686974207468652062756c6c277320657965";
    char expected[] = "746865206b696420646f6e277420706c6179";

    uint8_t* hex1 = str_to_hex_bytes((uint8_t *)input1, strlen(input1));
    uint8_t* hex2 = str_to_hex_bytes((uint8_t *)input2, strlen(input1));

    uint8_t* xored = fixed_xor(hex1, hex2, strlen(input1) / 2);
    uint8_t* result = hex_bytes_to_str(xored, strlen(input1) / 2);

    printf("Fixed xor : %s\n", strcmp(expected, (char*) result) == 0 ? "Ok" : "Error");

    free(hex1);
    free(hex2);
    free(xored);
    free(result);
}

void challenge3(void) {
    char input[] = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";
    size_t len = strlen(input);
    uint8_t key;

    uint8_t* cipher = str_to_hex_bytes((uint8_t *)input, len);
    double score = single_byte_xor_cipher(cipher, &key, len / 2);
    uint8_t* key_str = calloc(len / 2 + 1, sizeof(uint8_t));
    memset(key_str, key, len / 2);

    uint8_t* decrypted = fixed_xor(cipher, key_str, len / 2);

    printf("Decrypted = %s, key = %c, score = %.2lf\n", (char *)decrypted, (char)key, score);

    free(cipher);
    free(key_str);
    free(decrypted);
}

void challenge4(void) {
    char filename[] = "input_files\\4.txt";

    size_t len = 60;
    char* decrypted = calloc(len / 2 + 1, sizeof(char));
    char key;

    double score = detect_single_char_xor(filename, decrypted, &key, len);

    printf("Decrypted = %s, key = %c, score = %.2lf\n", decrypted, key, score);

    free(decrypted);
}

void challenge5(void) {
    char input_file[] = "input_files\\5.txt";
    char key[] = "ICE";

    FILE *f = fopen(input_file, "r");
    if (f == NULL) {
        perror("Erreur fopen");
        return;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = calloc(size, sizeof(char));
    size_t rd = fread(buffer, sizeof(char), size, f);

    uint8_t *decrypted_bytes = repeating_key_xor((uint8_t *)buffer, rd, (uint8_t *)key, strlen(key));
    uint8_t *decrypted_str = hex_bytes_to_str(decrypted_bytes, rd);

    printf("Repeating key xor : %s\n", (char*)decrypted_str);

    free(buffer);
    free(decrypted_bytes);
    free(decrypted_str);
    fclose(f);
}

void challenge6(void) {
    char input_file[] = "input_files\\6.txt";

    FILE *f = fopen(input_file, "r");
    if (f == NULL) {
        perror("Erreur fopen");
        return;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buffer = calloc(size + 1, sizeof(char));
    size_t rd = fread(buffer, sizeof(char), size, f);
    size_t real_len;
    clean_spaces(buffer, rd, &real_len);

    size_t b64_out_len;
    size_t key_len = 0;;

    uint8_t* hex_bytes = base64_to_hex_bytes((uint8_t*) buffer, real_len, &b64_out_len);
    uint8_t* key = break_repeating_key_xor(hex_bytes, b64_out_len, &key_len);

    printf("%lld\n", key_len);

    uint8_t* decrypted = repeating_key_xor(hex_bytes, b64_out_len, key, key_len);

    printf("Decrypted = %s\nkey = %s\n", (char *)decrypted, (char *)key);

    free(buffer);
    free(hex_bytes);
    fclose(f);
    free(key);
    free(decrypted);
}
