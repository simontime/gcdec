#include <stdio.h>
#include <stdint.h>
#include <string.h>

uint32_t decompress(uint8_t *in, uint8_t *out, uint32_t inLen)
{
	uint8_t *inEnd, *outStart;
	uint16_t copy;
	
	inEnd    = in + inLen;
	outStart = out;
	
	while (in < inEnd)
	{
		copy = 0;
		
		if (*in == 0xc0) // 16-bit RLE
		{
			copy = (in[2] << 8) | in[1];
			memset(out, in[3], copy);
			in += 4;
		}
		else if (*in > 0xc0) // 8-bit RLE
		{
			copy = *in & 0x3f;
			memset(out, in[1], copy);
			in += 2;
		}
		else // raw copy
		{
			*out++ = *in++;
		}
		
		out += copy;
	}
	
	return out - outStart;
}

int main(int argc, char **argv)
{
	FILE *in, *out;
	// data shouldn't ever be bigger than the size of a graphic bank
	uint8_t inBuf[0x4000];
	uint8_t outBuf[0x4000];
	uint32_t inLen, outLen;
	
	if (argc != 3)
	{
		printf("Usage: %s in out\n", argv[0]);
		return 0;
	}
	
	if ((in = fopen(argv[1], "rb")) == NULL)
	{
		perror("Error opening input");
		return 1;
	}
	
	if ((out = fopen(argv[2], "wb")) == NULL)
	{
		perror("Error opening output");
		return 1;
	}
	
	fseek(in, 0, SEEK_END);
	inLen = ftell(in);
	rewind(in);
	fread(inBuf, 1, inLen, in);
	fclose(in);
	
	outLen = decompress(inBuf, outBuf, inLen);
	fwrite(outBuf, 1, outLen, out);
	fclose(out);
	
	puts("Done!");
	
	return 0;
}