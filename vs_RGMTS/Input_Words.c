#include "rgmts.h"

int *Init_Words_List(int lens)
{
	int *list_t = NULL;
	list_t = Malloc(lens * sizeof(int));
	return list_t;
}

void Set_Words_List(int *lists, int p, int v, int lens)
{
	assert(p >= 0 && p < lens);
	lists[p] = v;
}
