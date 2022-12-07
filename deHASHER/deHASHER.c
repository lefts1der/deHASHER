// deHASHER.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#define _CRT_SECURE_NO_WARNINGS
#include "md5_hash.h"
#include "chars_and_digits.h"

void make_str(unsigned char* a, int index) {
    int i = 0;
    do  {
        a[i] = CHARS[index % 62];
        i++;
        index /= 62;
        index--;
        
    } while (index!=-1);
   
    
}



int main()
{
    char orig_hash[33];
    scanf("%32s", orig_hash);
    for (int i = 0; i < 916132832; i++) {
        unsigned char a[5];
        memset(a, 0, 5);
        make_str(a, i);

        MD5* md = init_md(a, strlen(a));
        
        hash(md);
        
        if (strcmp(md->hash, orig_hash)==0) {
            printf("%s", a);
            destruct(md);
            break;
        }
        destruct(md);
        
    }


}


