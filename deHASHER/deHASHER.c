// deHASHER.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#define _CRT_SECURE_NO_WARNINGS

#include "md5_hash.h"
#include "chars_and_digits.h"





int maisn()
{
    char orig_hash[33];
    MD5* md = init_md("A", 1);

    hash(md);
    printf("%s", md->hash);

    scanf("%s", orig_hash);

    for (uint64_t i = 0; i < 839299365868340000; i++) {
        unsigned char a[10];
        memset(a, 0, 10);
        make_str(a, i);

        MD5* md = init_md(a, strlen(a));

        hash(md);

        if (strcmp(md->hash, orig_hash) == 0) {
            printf("%s", a);
            destruct(md);
            break;
        }
        destruct(md);
        

    }
    

}





