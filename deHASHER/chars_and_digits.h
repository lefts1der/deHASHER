#pragma once

static const unsigned char CHARS[]= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz01234567890";

void make_str(unsigned char* a, uint64_t index) {
    int i = 0;
    do {
        a[i] = CHARS[index % 62];
        i++;
        index /= 62;
        index--;

    } while (index != -1);


}