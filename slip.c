#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* 本程序主要实现一下slip协议的编码和解码过程 */

unsigned char seeds[16] = {0xc0, 0x5a, 0xa3, 0xf7, 
						   0x66, 0xdb, 0x09, 0xdd, 
						   0x21, 0xdc, 0xdb, 0xc0, 
						   0x25, 0x8e, 0x99, 0xdd};
#define SEEDS_LEN  sizeof(seeds)/sizeof(seeds[0])

void print_encode_ret (const unsigned char* ret)
{
	int idx = 1;
	printf ("0x%x ", ret[0]);
	while (ret[idx] != 0xc0)
	{
		printf ("0x%x ", ret[idx]);
		++idx;
	}
	printf ("0x%x\n", ret[idx]);
	return ;
}

void encode_slip (const unsigned char* src, unsigned char *result, int len)
{
	int src_idx, ret_idx;

	if (NULL == src || NULL == result)
		return;

	result[0] = 0xc0;
	for (src_idx = 0, ret_idx = 1; src_idx < len; ++src_idx)
	{
		if (src[src_idx] == 0xc0)
		{
			result[ret_idx] = 0xdb;
			result[ret_idx+1] = 0xdc;
			ret_idx += 2;
		}
		else if (src[src_idx] == 0xdb)
		{
			result[ret_idx] = 0xdb;
			result[ret_idx+1] = 0xdd;
			ret_idx += 2;
		}
		else
		{
			result[ret_idx++] = src[src_idx];
		}
	}
	result[ret_idx] = 0xc0;
	return ;
}

void decode_slip (const unsigned char* src, unsigned char *result, int *ret_len)
{
	int src_idx = 1;
	int ret_idx = 0;
	while (src[src_idx] != 0xc0)
	{
		if (src[src_idx] == 0xdb)
		{
			if (src[src_idx+1] == 0xdc)
			{
				result[ret_idx++] = 0xc0;
				src_idx += 2;
			}
			else if (src[src_idx+1] == 0xdd)
			{
				result[ret_idx++] = 0xdb;
				src_idx += 2;
			}
			else
			{
				result[ret_idx++] = 0xdb;
				src_idx += 1;
			}
		}
		else
		{
			result[ret_idx++] = src[src_idx++];
		}
	}
	*ret_len = ret_idx;
	return;
}

void print_arr (const unsigned char* ret, int len)
{
	int idx = 0;
	for (idx = 0; idx < len; ++idx)
	{
		printf ("0x%x ", ret[idx]);
	}
	printf ("\n");
}
	

int main (void)
{
	srand (time(NULL));
	int decode_len = 0;
	int idx = 0;
	int origin_len = rand () % 30 + 1;
	unsigned char *arr = (unsigned char*)malloc (sizeof (char) * origin_len);

	for (idx = 0; idx < origin_len; ++idx)
	{
		arr[idx] = seeds[rand()%SEEDS_LEN];
	}

	printf ("src       : ");
	print_arr (arr, origin_len);

	unsigned char encode_ret[128] = {0x0};
	unsigned char decode_ret[128] = {0x0};

	encode_slip (arr, encode_ret, origin_len);
	printf ("encode ret: ");
	print_encode_ret (encode_ret);
	
	decode_slip (encode_ret, decode_ret, &decode_len);
	printf ("decode ret: ");
	print_arr (decode_ret, decode_len);

	return 0;
}
