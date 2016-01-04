#include "utils.h"


void doXor2Arrays(char *source, int srcOff, char *result, int resOff, int length)
{
	int srcIndex = srcOff;
	int resIndex = resOff;

	int i = 0;

	for (i = 0; i < length; i++)
	{
		result[resIndex] = (char)(source[srcIndex] ^ result[resIndex]);
		srcIndex++;
		resIndex++;
	}
}

int char2int(char input)
{
	if (input >= '0' && input <= '9')
		return input - '0';
	if (input >= 'A' && input <= 'F')
		return input - 'A' + 10;
	if (input >= 'a' && input <= 'f')
		return input - 'a' + 10;

	return -1;
}

char int2char(char input)
{
	if (input == 0x00)
		return '0';
	else if (input == 0x01)
		return '1';
	else if (input == 0x02)
		return '2';
	else if (input == 0x03)
		return '3';
	else if (input == 0x04)
		return '4';
	else if (input == 0x05)
		return '5';
	else if (input == 0x06)
		return '6';
	else if (input == 0x07)
		return '7';
	else if (input == 0x08)
		return '8';
	else if (input == 0x09)
		return '9';
	else if (input == 0x0a)
		return 'A';
	else if (input == 0x0b)
		return 'B';
	else if (input == 0x0c)
		return 'C';
	else if (input == 0x0d)
		return 'D';
	else if (input == 0x0e)
		return 'E';
	else if (input == 0x0f)
		return 'F';

	return -1;
}

void hex2bin(unsigned char* target, const unsigned char* src, int srcLen)
{
	for (int i = 0; i < srcLen; i = i + 2)
	{
		*(target++) = char2int(*src) * 16 + char2int(src[1]);
		src += 2;
	}
}


/*
	Convert bytes to hex 
*/
int bin2Hex(unsigned char* target, const unsigned char* src, int srcLen)
{
	for (int i = 0; i < srcLen; i++)
	{
		*(target) = int2char(*src >> 4);
		target++;
		*(target) = int2char(*src & 0x0f);

		target++;
		src++;
	}

	return srcLen * 2;
}

