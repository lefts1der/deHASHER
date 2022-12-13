#define _CRT_SECURE_NO_WARNINGS
#include "md5_hash.h"

MD5* init_md(const unsigned char* original_input, uint8_t length) {
    MD5* md5_new = (MD5*)malloc(sizeof(MD5));
    md5_new->length = length;

    md5_new->word = (char*)calloc(length , sizeof(unsigned char));
    memcpy(md5_new->word, original_input, length);


    md5_new->buffers[0] = (uint32_t)A;
    md5_new->buffers[1] = (uint32_t)B;
    md5_new->buffers[2] = (uint32_t)C;
    md5_new->buffers[3] = (uint32_t)D;

    return md5_new;
}

// Add '1' bit and '0' bit to 56 length
static void align_md(MD5* md5) {
    memcpy(md5->ready_word, md5->word, md5->length);
    md5->ready_word[md5->length] = 128;
    for (uint8_t i = md5->length + 1; i < 56; i++) md5->ready_word[i] = 0;
}

// Add original length as bits
static void add_lenth(MD5* md5) {
    uint32_t bit_length = md5->length * 8;
    memcpy(md5->ready_word + 56, &bit_length, 4);   // Add first 4 bytes
    for (int i = 60; i < 64; i++)md5->ready_word[i] = 0;    // Add last 4 bytes
}

static uint32_t rotateLeft(uint32_t x, uint32_t n) {
    return (x << n) | (x >> (32 - n));
}

static normalize_hash(MD5* md5) {
    for (unsigned int i = 0; i < 4; ++i) {
        
        sprintf(md5->hash + ((i * 8) + 0), "%02x", (uint8_t)((md5->buffers[i] & 0x000000FF)));
        sprintf(md5->hash + ((i * 8) + 2), "%02x", (uint8_t)((md5->buffers[i] & 0x0000FF00) >> 8));
        sprintf(md5->hash + ((i * 8) + 4), "%02x", (uint8_t)((md5->buffers[i] & 0x00FF0000) >> 16));
        sprintf(md5->hash + ((i * 8) + 6), "%02x", (uint8_t)((md5->buffers[i] & 0xFF000000) >> 24));
        
        
    }
    
}

void hash(MD5* md5) {
    align_md(md5);
    add_lenth(md5);


    uint64_t AA = md5->buffers[0];
    uint64_t BB = md5->buffers[1];
    uint64_t CC = md5->buffers[2];
    uint64_t DD = md5->buffers[3];

    uint32_t X[16];
    memcpy(X, md5->ready_word, 64);
    for (int i = 0; i < 16; i++)printf("%d\n", X[i]);



    for (uint8_t i = 0; i < 64; i++) {
        uint32_t TMP;
        uint8_t k;

        switch (i / 16)
        {
            // FUNCTION F
        case(0): {
            TMP = F(BB, CC, DD);
            k = i;
            break;
        }
        case(1): {
            TMP = G(BB, CC, DD);
            k = ((i * 5) + 1) % 16;
            break;

        }
        case(2): {
            TMP = H(BB, CC, DD);
            k = ((i * 3) + 5) % 16;
            break;

        }

        default:
            TMP = I(BB, CC, DD);
            k = (i * 7) % 16;
            break;
        }
        TMP += K[i] + X[k] + AA;
        AA = DD;
        DD = CC;
        CC = BB;
        BB += rotateLeft(TMP, S[i]);
    }
    md5->buffers[0] += AA;
    md5->buffers[1] += BB;
    md5->buffers[2] += CC;
    md5->buffers[3] += DD;
    printf("%02x\n", md5->buffers[0]);
    printf("%02x\n", md5->buffers[1]);
    printf("%02x\n", md5->buffers[2]);
    printf("%02x\n", md5->buffers[3]);

    
    normalize_hash(md5);
    
}

void destruct(MD5* md5) {
    free(md5->word);
    free(md5);
}