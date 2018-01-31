#include "rgmts.h"
int is_show = 0;

Keys* Client_Init(int document_num, int words_num, int dimension, int extend, int extend_num, int key_len)
{
	Keys *k_temp;
	int d_extend = extend_num*key_len / 2;
	int arr_used[2] = { 1,3 };
	srand((unsigned int)time(0));
	k_temp = Keys_Init(document_num, words_num, dimension, extend, extend_num, key_len, d_extend, arr_used);
	return k_temp;
}

