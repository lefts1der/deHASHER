#define _CRT_SECURE_NO_WARNINGS
#include <stdbool.h>
#include <ctype.h> 
#include <limits.h>
#include <stdint.h>

#define DEFAULT_BLOCKS 40495468725
#define DEFAULT_BLOCK_SIZE 331612160

enum {
	HELP=-1,
	SUCCESS,
	INCORRECT_DEVICE,
	INCORRECT_BLOCKS,
	INCORRECT_BLOCKS_SIZE,
	BLOCKS_CPU,
	UNKNOWN_ARG,
	INCORRECT_HASH,
	INCORRECT_MULTIPLY,
} ERR_CODE = SUCCESS;


void print_info();
void print_err();
bool is_hash_correct(char*);

unsigned char* deHASHER_CPU(unsigned char orig_hash[33]);
unsigned char* deHASHER_GPU(unsigned char orig_hash[33],
	uint64_t BLOCKS, uint64_t BLOCKS_SIZE);


int main(int argc, char* argv[]) {

	if (argc==1 ||
		!strcmp(argv[1],"--help")) {
		print_info();
		return HELP;
	}
	enum { CPU, GPU } dehasher_device = GPU;
	

	unsigned long long BLOCKS = DEFAULT_BLOCKS;
	unsigned long long BLOCKS_SIZE = DEFAULT_BLOCK_SIZE;

	unsigned char orig_hash[33] = {0};
	unsigned char* result;

	for (int cur_arg = 1; cur_arg < argc && argv[cur_arg][0]=='-'; cur_arg++) {
		switch (argv[cur_arg][1])
		{
		case 'd':
			cur_arg++;
			if (!strcmp(argv[cur_arg], "CPU")) {
				dehasher_device = CPU;
				BLOCKS = 0;
				BLOCKS_SIZE = 0;
			}
			else if (!strcmp(argv[cur_arg], "GPU")) dehasher_device = GPU;
			else ERR_CODE = INCORRECT_DEVICE;
			break;
		case 'b':
			cur_arg++;
			if (argv[cur_arg][0] == '-' || argv[cur_arg][0] == '0') ERR_CODE = INCORRECT_BLOCKS;
			BLOCKS = strtoull(argv[cur_arg], NULL, 10);
			break;
		case 't':
			cur_arg++;
			if (argv[cur_arg][0] == '-' || argv[cur_arg][0] == '0') ERR_CODE = INCORRECT_BLOCKS_SIZE;
			BLOCKS_SIZE = strtoull(argv[cur_arg], NULL, 10);
			break;
		case 'h':
			cur_arg++;
			if (is_hash_correct(argv[cur_arg])) memcpy(orig_hash, argv[cur_arg], 32);
			else ERR_CODE = INCORRECT_HASH;
			break;
		default:
			ERR_CODE = UNKNOWN_ARG;
			break;
		}
		if (ERR_CODE) {
			print_err();
			return ERR_CODE;
		}
	}

	if (dehasher_device == CPU && (BLOCKS != 0 || BLOCKS_SIZE != 0)) {
		ERR_CODE = BLOCKS_CPU;
		print_err();
		return ERR_CODE;
	}
	if (dehasher_device == GPU && ULLONG_MAX / BLOCKS < BLOCKS_SIZE) {
		ERR_CODE = INCORRECT_MULTIPLY;
		print_err();
		return ERR_CODE;
	}

	if (dehasher_device == CPU){
		printf("Device: CPU\n");
		result = deHASHER_CPU(orig_hash);
	}
	else {
		printf("Device: GPU\n");
		printf("Blocks: %llu\n", BLOCKS);
		printf("Threads in block: %llu\n\n\n", BLOCKS_SIZE);
		result = deHASHER_GPU(orig_hash, BLOCKS, BLOCKS_SIZE);
	}
	printf("\n\nResult: %s\n", result);
	free(result);
}

bool is_hash_correct(char* hash) {
	if (hash == NULL) return false;
	if (strlen(hash) != 32) return false;
	for (int i = 0; i < 32; i++) {
		hash[i] = tolower(hash[i]);
		if (hash[i] != '0' && hash[i] != '1' && hash[i] != '2' && hash[i] != '3' &&
			hash[i] != '4' && hash[i] != '5' && hash[i] != '6' && hash[i] != '7' &&
			hash[i] != '8' && hash[i] != '9' && hash[i] != 'a' && hash[i] != 'b' &&
			hash[i] != 'c' && hash[i] != 'd' && hash[i] != 'e' && hash[i] != 'f') return false;
	}
	return true;
}
void print_info() {
	printf("Usage: deHASHER.exe [OPTIONS]...\n\n");
	printf("Options:\n");
	printf("  -h TEXT \t The string of the hash to be dehashed. Only hex record. Required parameter.\n");
	printf("  -d TEXT [CPU/GPU] \t The device on which the dehashing is performed \
[default: CPU]\n");
	printf("  -b NUMBER \t The count of blocks to be iterated on GPU. Count of attempts to dehash depend on this arg. \
Require to select GPU. [default: 40495468725]\n");
	printf("  -t NUMBER \t The count of threads in one block. \
Require to select GPU. [default: 40495468725]\n");
	printf("\n\n");
	printf("The product of the number of blocks by the number of threads in the block\n\
must be greater than or equal to 62^n, where n is the length of the hashed word, \n\
and less then unsigned long long`s max\n");

}
void print_err() {
	switch (ERR_CODE)
	{
	case INCORRECT_DEVICE:
		printf("It is necessary to choose between only \"CPU\" or \"GPU\"!\n");
		break;
	case INCORRECT_BLOCKS:
		printf("The blocks count is needed to be positive!\n");
		break;
	case INCORRECT_BLOCKS_SIZE:
		printf("The count of threads in the one block is needed to be positive!\n");
		break;
	case BLOCKS_CPU:
		printf("Blocks and its size need to bre unsetted!\n\
It is useless parameters on GPU.\n");
		break;
	case UNKNOWN_ARG:
		printf("Unknown argument!\n");
		break;
	case INCORRECT_HASH:
		printf("Incorrect hash!\n");
		break;
	case INCORRECT_MULTIPLY:
		printf("Incorrect parameters of counts threads and blocks!\n");
		break;
	}
	printf("Check \"deHASHER.exe --help\" to more information.\n");
}









