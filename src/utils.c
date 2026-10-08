#include "utils.h"

#define MAX_BUFF 2048

char hex_chars[16] = "0123456789abcdef";
char base64_enc_lut[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
float en_freq[26] = {8.167, 1.492, 2.782, 4.253, 12.702, 2.228, 2.015, 6.094, 6.966, 0.153, 0.772, 4.025, 2.406,
                    6.749, 7.507, 1.929, 0.095, 5.987,  6.327, 9.056, 2.758, 0.978, 2.36,  0.15,  1.974, 0.074};
char base64_dec_lut[128] = {
    80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, /* 0 - 15 */
    80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, /* 16 - 31 */
    80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 62, 80, 80, 80, 63, /* 32 - 47 */
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 80, 80, 80, 64, 80, 80, /* 48 - 63 */
    80,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, /* 64 - 79 */
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 80, 80, 80, 80, 80, /* 80 - 96 */
    80, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, /* 87 - 111 */
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 80, 80, 80, 80, 80 /* 112 - 127 */
}; 

uint8_t char_to_int(char c) {
    if ('0' <= c && c <= '9') {
        return c - '0';
    } else {
        return c - 'a' + 10;
    }
}

char int_to_char(uint8_t i) {
    if (i <= 9) {
        return i + '0';
    } else {
        return i - 10 + 'a';
    }
}

/* 
Tableau d'octets hexadécimaux
['1', 'b', '3', '7'] -> [0x1b, 0x37]
*/
uint8_t* str_to_hex_bytes(uint8_t* str, size_t len) {
    uint8_t *buffer = calloc(len / 2, sizeof(uint8_t));
    if (!buffer) return NULL;

    for (size_t i = 0; i < len; i += 2) {
        uint8_t high = char_to_int(str[i]);
        uint8_t low = char_to_int(str[i + 1]);

        buffer[i / 2] = (high << 4 | low);
    }

    return buffer;
}

/* 
Représentation textuelle hexadécimale
[0x1b, 0x37] -> ['1', 'b', '3', '7'] 
*/
uint8_t* hex_bytes_to_str(uint8_t *hex, size_t len) {
    size_t new_len = len * 2 + 1;
    uint8_t *buffer = calloc(new_len, sizeof(uint8_t));
    if (!buffer) return NULL;

    for (size_t i = 0; i < len; i++) {
        buffer[2 * i] = int_to_char(hex[i] >> 4);
        buffer[2 * i + 1] = int_to_char(hex[i] & 0b1111);
    }

    return buffer;
}

/*
https://www.sunshine2k.de/articles/coding/base64/understanding_base64.html

On convertit à chaque fois 3 octets hexadécimaux (24 bits) en 4 caractères base64 de 6 bits (24 bits).
Exemple avec "Sun":
01010011 01110101 01101110 => 010100 110111 010101 101110

Pour le dernier triplet non complet, il reste soit un octet soit deux octets.
On convertit en caractères base64 de 6 bits en complétant par du padding, puis on ajoute les '='.
*/
uint8_t* hex_bytes_to_base64(uint8_t *hex, size_t len, size_t *out_len) {
    size_t new_len = ((len + 2) / 3) * 4;
    size_t index_of_last_triple = (int)(len / 3) * 3;
    size_t src_idx = 0;
    size_t dst_idx = 0;
    uint8_t *buffer = calloc(new_len, sizeof(uint8_t));

    for (src_idx = 0; src_idx < index_of_last_triple; src_idx += 3) {
        uint8_t o1 = hex[src_idx];
        uint8_t o2 = hex[src_idx + 1];
        uint8_t o3 = hex[src_idx + 2];

        buffer[dst_idx] = base64_enc_lut[o1 >> 2]; // 6 bits octet 1
        buffer[dst_idx + 1] = base64_enc_lut[((o1 & 0b11) << 4) | (o2 >> 4)]; // 2 bits octet 1 + 4 bits octet 2
        buffer[dst_idx + 2] = base64_enc_lut[((o2 & 0b1111) << 2) | (o3 >> 6)]; // 2 bits octet 2 + 4 bits octet 3
        buffer[dst_idx + 3] = base64_enc_lut[o3 & 0b111111]; // 6 bits octet 3

        dst_idx += 4;
    }

    if (src_idx < len) {
        uint8_t o1 = hex[src_idx]; // Le premier octet restant est toujours disponible
        uint8_t o2 = (src_idx + 1) < len ? hex[src_idx + 1] : (uint8_t)0; // Soit deuxième octet disponible, soit remplissage par 0.
        
        buffer[dst_idx] = base64_enc_lut[o1 >> 2]; // 6 bits du premier octet
        buffer[dst_idx + 1] = base64_enc_lut[((o1 & 0b11) << 4) | (o2 >> 4)]; // 2 bits octet 1 et (4 bits octets 2 OU padding 0)
        
        if (src_idx + 1 < len) {
            buffer[dst_idx + 2] = base64_enc_lut[(o2 & 0b1111) << 2]; // 4 bits restants octet 2 et padding de 0
            buffer[dst_idx + 3] = '=';
        } else {
            buffer[dst_idx + 2] = '='; // Un seul octet restant, 2 caractères '=' au total
            buffer[dst_idx + 3] = '=';
        }
    }

    *out_len = new_len;

    return buffer;
}

/*
https://www.sunshine2k.de/articles/coding/base64/understanding_base64.html

On convertit à chaque fois 4 caractères base64 de 6 bits (24 bits) 
en 3 octets hexadécimaux (24 bits).

Exemple avec "Sun":
010100 110111 010101 101110 => 01010011 01110101 01101110
Pour extraire les caractères, on a juste à faire une recherche inversée dans la table.

Pour le dernier quadruplet non complet, il y a soit à la fin un seul caractère '=' de padding, soit deux.
Dans ce cas c'est très simple, on ne convertit que les octets complets qu'il est possible de former avec les groupes
de 6 bits. Les autres sont ignorés.
*/
uint8_t* base64_to_hex_bytes(uint8_t *b64, size_t len, size_t *out_len) {
    if ((len < 4) || (len % 4 != 0)) return NULL;

    size_t new_len = (len / 4) * 3;
    size_t last_complete_idx = 0;

    if (b64[len - 1] == '=') {
        last_complete_idx = ((len / 4) - 1) * 4;
    } else {
        last_complete_idx = (len / 4) * 4;
    }

    if (b64[len - 1] == '=') new_len--;
    if (b64[len - 2] == '=') new_len--;

    size_t src_idx = 0;
    size_t dst_idx = 0;
    uint8_t *buffer = calloc(new_len, sizeof(uint8_t));

    for (src_idx = 0; src_idx < last_complete_idx; src_idx += 4) {
        uint8_t b1 = base64_dec_lut[b64[src_idx]];
        uint8_t b2 = base64_dec_lut[b64[src_idx + 1]];
        uint8_t b3 = base64_dec_lut[b64[src_idx + 2]];
        uint8_t b4 = base64_dec_lut[b64[src_idx + 3]];

        // Attention les 6 bits base64 sont quand même encodés sur 8 bits (2 zéros à gauche)
        buffer[dst_idx] = (b1 << 2) | (b2 >> 4);
        buffer[dst_idx + 1] = ((b2 & 0x0F) << 4) | (b3 >> 2);
        buffer[dst_idx + 2] = ((b3 & 0x03) << 6) | b4;

        dst_idx += 3;
    }

    if (b64[len - 1] == '=') {
        uint8_t b1 = base64_dec_lut[b64[src_idx]];
        uint8_t b2 = base64_dec_lut[b64[src_idx + 1]];

        buffer[dst_idx] = (b1 << 2) | (b2 >> 4);

        if (b64[src_idx + 2] != '=') {
            uint8_t b3 = base64_dec_lut[b64[src_idx + 2]];
            buffer[dst_idx + 1] = ((b2 & 0x0F) << 4) | (b3 >> 2);
        }
    }

    *out_len = new_len;

    return buffer;
}

uint8_t* fixed_xor(uint8_t* buffer1, uint8_t* buffer2, size_t len) {
    uint8_t* result = calloc(len + 1, sizeof(uint8_t));

    for (size_t i = 0; i < len; i++) {
        result[i] = buffer1[i] ^ buffer2[i];
    }

    return result;
}

int letter_to_index(uint8_t c) {
    if ('a' <= c && c <= 'z') {
        return c - 'a';
    } else if ('A' <= c && c <= 'Z') {
        return c - 'A';
    } else {
        return -1;
    }
}

bool is_printable_char(uint8_t c) {
    return (c >= 32 && c <= 126) || (c == '\n') || (c == '\t') || (c == '\r');
}

bool compute_pruning(uint8_t chr, int* letterCount, int *count) {
    if (!is_printable_char(chr)) {
        return false;
    }

    int idx = letter_to_index(chr);
    if (idx != -1) {
        count[idx]++;
        (*letterCount)++;
    }

    return true;
}

/*
Calcule la distance entre les fréquences de chaque lettre pour la langue anglais et le texte.
La somme vaut au maximum 200 en théorie (Valeur max de 100% pour freq et de 100% pour en_freq)
*/
double compute_dist(double *freq) {
    double sum = 0.0;

    for (int i = 0; i < 26; i++) {
        sum += fabs(en_freq[i] - freq[i]);
    }

    return 100.0 - (sum / 2.0); // On ramène la distance sur 100

}

double single_byte_xor_cipher(uint8_t* cipher, uint8_t* key, size_t len) {
    double best_score = INT_MIN;
    char best_char = 0;
    
    for (int chr = 0; chr < 128; chr++) {
        int count[26] = {0};
        double freq[26];
        int letterCount = 0;
        bool is_valid_char = 1;

        for (size_t j = 0; j < len; j++) {
            uint8_t xor_char = cipher[j] ^ (uint8_t)chr; // uint8_t est un char non signé
            if (!(is_valid_char = compute_pruning(xor_char, &letterCount, count))) {
                break;
            }
        }

        if (letterCount <= 0 || !is_valid_char) continue;

        for (int k = 0; k < 26; k++) {
            freq[k] = ((double) count[k] / letterCount) * 100;
        }

        double score = compute_dist(freq);

        if (score > best_score) {
            best_score = score;
            best_char = (char)chr;
        }
    }

    *key = best_char;

    return best_score;
}

double detect_single_char_xor(char* filename, char* decrypted, char* key, size_t len) {
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        perror("Erreur fopen");
        return 0;
    }

    char* buffer = calloc(len + 1, sizeof(char));
    double best_score = -DBL_MAX;
    uint8_t best_char = 0;

    while (fscanf(f, "%60s", buffer) == 1) {
        uint8_t current_key = 0;
        uint8_t *cipher = str_to_hex_bytes((uint8_t*)buffer, len);
        double score = single_byte_xor_cipher(cipher, &current_key, len / 2);

        if (score > best_score) {
            best_score = score;
            best_char = current_key;
            
            for (size_t i = 0; i < len / 2; i++) {
                decrypted[i] = (char)(cipher[i] ^ best_char);
            }
        }
        free(cipher);
    }

    *key = (char)best_char;
    decrypted[strcspn(decrypted, "\n")] = '\0';

    fclose(f);
    free(buffer);

    return best_score;
}

uint8_t* repeating_key_xor(uint8_t* text, size_t size, uint8_t* key, size_t key_len) {
    uint8_t *buffer = calloc(size + 1, sizeof(uint8_t));

    for (size_t i = 0; i < size; i++) {
        buffer[i] = text[i] ^ key[i % key_len];
    }

    return buffer;
}

/*
Soit x = 10 11 01 10. On veut compter le nombre de bits à 1 (5).

La première opération consiste à additionner les bits 2 à 2
(x >> 1) & 0x55 = 01 01 10 11 & 01 01 01 01 = 01 01 00 01 (1 1 0 1)
 x       & 0x55 = 00 01 01 00 (0 1 1 0)
La somme vaut x = 01 10 01 01 (1 2 1 1) soit le nombre de 1 dans chaque paire
     
Puis on additionne les bits 4 à 4.
(x >> 2) & 0x33 = 00 01 10 01 & 00 11 00 11 = 00 01 00 01 (0 1 0 1)
 x       & 0x33 = 01 10 01 01 & 00 11 00 11 = 00 10 00 01 (0 2 0 1)
La somme vaut x =  00 11 00 10 (0 3 0 2)

Enfin on additionne le reste
(x >> 4) & 0x0F = 00 00 00 11 & 00 00 11 11 = 00 00 00 11 (0 0 0 3)
 x       & 0x0F = 00 11 00 10 & 00 00 11 11 = 00 00 00 10 (0 0 0 2)
On obtient bien 0b11 + 0b10 = 3 + 2 = 5

*/
uint8_t bit_count(uint8_t x) {
    uint8_t y = ((x >> 1) & 0x55) + (x & 0x55);
    y = ((y >> 2) & 0x33) + (y & 0x33);
    y = ((y >> 4) & 0x0F) + (y & 0x0F);

    return y;
}

int hamming_distance(uint8_t* str1, uint8_t* str2, size_t len) {    
    int differing_bits = 0;
    
    for (size_t i = 0; i < len; i++) {
        uint8_t x = (uint8_t) str1[i] ^ str2[i]; // Le XOR ne conserve que les bits différents
        differing_bits += bit_count(x);
    }

    return differing_bits;
}

size_t break_repeating_key_size(uint8_t* cipher, size_t len) {
    size_t best_key_size = 2;
    double best_dist = DBL_MAX;

    for (size_t key_size = 2; key_size <= 40; key_size++) {
        if (len < 4 * key_size) break;

        int b1b2_dist = hamming_distance(cipher, cipher + key_size, key_size); 
        int b2b3_dist = hamming_distance(cipher + key_size, cipher + 2 * key_size, key_size);
        int b3b4_dist = hamming_distance(cipher + 2 * key_size, cipher + 3 * key_size, key_size); 

        double dist = (double)(b1b2_dist + b2b3_dist + b3b4_dist) / (3.0 * (double)key_size);

        if (dist < best_dist) {
            best_dist = dist;
            best_key_size = key_size;
        }
    }

    return best_key_size;
}

/*
Deux choses à faire :
1. Casser la longueur de la clé
2. Casser la clé (trouver les caractères)

Compréhension supposée du problème :
Puisqu'on suppose que le message a été chiffré avec du xor, soit deux blocs chiffrés consécutifs
de longueur K (C1 et C2) issus de deux blocs de texte clair (P1 et P2) chiffrés avec la même clé K.
C1 = P1 ^ K
C2 = P2 ^ K
=> C1 ^ C2 = P1 ^ K ^ P2 ^ K = P1 ^ P2.
Cela revient à calculer la distance de Hamming entre deux morceaux de texte clair.
Dans les langues, elle est statistiquement plus faible que dans du bruit (les morceaux de clé décalés).
On donc donc former des blocs de la taille de la clé.

Puisque chaque caractère de même rang dans chaque bloc est chiffré par le même charactère de la clé,
il ne reste plus qu'à faire une étude statistque sur ces blocks et concaténer les caractères trouvés de la clé.
*/
uint8_t* break_repeating_key_xor(uint8_t* cipher, size_t len, size_t* key_len) {
    size_t key_size = break_repeating_key_size(cipher, len);
    *key_len = key_size;

    uint8_t *key = calloc(key_size + 1, sizeof(uint8_t));

    // Taille maximale d'une colonne transposée : ceil(len / key_size)
    size_t max_col_len = (len + key_size - 1) / key_size;
    uint8_t *buffer = calloc(max_col_len, sizeof(uint8_t));

    for (size_t j = 0; j < key_size; j++) {
        size_t col_len = 0;

        for (size_t i = j; i < len; i += key_size) {
            buffer[col_len++] = cipher[i];
        }

        uint8_t best_char = 0;
        single_byte_xor_cipher(buffer, &best_char, col_len);
        key[j] = best_char;
    }

    free(buffer);

    return key;
}
