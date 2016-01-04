#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#define GLOBAL_BUFFER_SIZE  1024*10

	void doXor2Arrays(char *source, int srcOff, char *result, int resOff, int length);
	int char2int(char input);
	void hex2bin(unsigned char* target, const unsigned char* src, int srcLen);
	char int2char(char input);
	int bin2Hex(unsigned char* target, const unsigned char* src, int srcLen);

#ifdef __cplusplus
}
#endif

