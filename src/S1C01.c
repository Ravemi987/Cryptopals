#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Challenge 1

void print_hex_array(uint8_t *hex_arr, size_t len) {
    printf("{ ");

    for (size_t i = 0; i < len; i++) {
        printf("0x%02x ", hex_arr[i]);
    }

    printf("}\n");
}

uint8_t char_to_int(char c) {
     if ('0' <= c && c <= '9') {
        return c - '0';
     } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
     } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
     } else {
        return -1;
     }
}

uint8_t* hexstr_to_hex(char* str, size_t len) {
    uint8_t *buffer = calloc(len/2, sizeof(uint8_t));
    if (!buffer) return NULL;

    for (size_t i = 0; i < len; i += 2) {
        uint8_t high = char_to_int(str[i]);
        uint8_t low = char_to_int(str[i + 1]);

        buffer[i / 2] = (high << 4 | low);
    }

    return buffer;
}

uint8_t hex_to_base64(uint8_t *hex, size_t len) {


}

int main(int argc, char* argv[]) {

    if (argc < 1) {
        return 0;
    }

    size_t len = strlen(argv[1]);

    uint8_t* hex_array = hexstr_to_hex(argv[1], len);
    print_hex_array(hex_array, len / 2);

    free(hex_array);

    return 0;
}
