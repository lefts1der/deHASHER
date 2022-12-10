#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_global_int64_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_int64_base_atomics : enable

__constant unsigned char CHARS[]= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz01234567890";

//#define normal(X, B, S) (((X & B) >> S))
unsigned char normal(unsigned int buffer, unsigned int bytes, unsigned char offset){
    return (unsigned short)(((buffer & bytes) >> offset));
}

// Buffers for calculating
#define A 0x67452301
#define B 0xefcdab89
#define C 0x98badcfe
#define D 0x10325476

// Functions on each rounds
#define F(X, Y, Z) ((X & Y) | (~X & Z))
#define G(X, Y, Z) ((X & Z) | (Y & ~Z))
#define H(X, Y, Z) (X ^ Y ^ Z)
#define I(X, Y, Z) (Y ^ (X | ~Z))

__constant unsigned char S[] = { 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                       5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
                       4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                       6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 };

// Array consts for each rounds
__constant unsigned int K[] = { 0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
                       0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
                       0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
                       0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
                       0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
                       0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
                       0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
                       0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
                       0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
                       0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
                       0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
                       0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
                       0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
                       0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
                       0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
                       0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };



__kernel void deHASH(__global  unsigned char* input, __global unsigned char* result){

long index = get_global_id(0);
if(index>=839299365868340224) return;


unsigned char length = 0;
unsigned char str[10];
for (int i=0; i<10; i++) str[i]='\0';
do {
        str[length] = CHARS[index % 62];
        length++;
        index /= 62;
        index--;

    } while (index != -1);
length++;

unsigned char main_str[64];

// align_md
for(int i=0; i<length; i++)main_str[i]=str[i];
main_str[length]=128;
for(int i = length+1; i<56;i++)main_str[i]=0;

// add length
main_str[56]=length*8;
for(int i = 57; i<64; i++) main_str[i]=0;

// main algorithm
unsigned int buffers[4];
buffers[0] = (unsigned int)A;
buffers[1] = (unsigned int)B;
buffers[2] = (unsigned int)C;
buffers[3] = (unsigned int)D;

unsigned int AA = buffers[0];
unsigned int BB = buffers[1];
unsigned int CC = buffers[2];
unsigned int DD = buffers[3];

unsigned int X[16];

for(int j =0; j<16;j++){
    X[j]=0;
    for(int i=0; i<4;i++)
    X[j]=(unsigned int)main_str[(j*16)+i]<<(i<<3);
}

for (unsigned char i = 0; i < 64; i++) {
        unsigned int TMP;
        unsigned char k;

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
        BB += (TMP << S[i]) | (TMP >> (32-S[i]));
    }

buffers[0] += AA;
buffers[1] += BB;
buffers[2] += CC;
buffers[3] += DD;

unsigned char result_int[16];

for(unsigned int i = 0; i < 4; i++){
        if((normal(buffers[i],0x000000FF, 0)  != input[i*4]) ||
        (normal(buffers[i],0x0000FF00 , 8) != input[i*4 +1]) ||
        (normal(buffers[i],0x00FF0000 , 16) != input[i*4 +2]) ||
        (normal(buffers[i],0xFF000000 , 24) != input[i*4 +3])) return;
	}
  
 
  result[1]=12;              

//for(int i =0; i<16; i++) if(result_int[i]!=input[i])return;
//for(unsigned int i=0;i<10;i++) ;

}
