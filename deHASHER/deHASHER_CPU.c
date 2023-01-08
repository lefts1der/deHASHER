#define _CRT_SECURE_NO_WARNINGS

#include "md5_hash.h"
#include "chars_and_digits.h"

unsigned char* deHASHER_CPU(unsigned char orig_hash[33])
{
    unsigned char* generated_str = (unsigned char*)malloc(11* sizeof(char));
    memset(generated_str, 0, 11);
    for (uint64_t i = 0; i < 839299365868340000; i++) {
        
        make_str(generated_str, i);

        MD5* md = init_md(generated_str, strlen(generated_str));

        hash(md);

        if (strcmp(md->hash, orig_hash) == 0) {           
            destruct(md);
            return generated_str;
        }
        destruct(md);


    }
}