#include "../include/example.h"

int reverse_boolean(struct boolean* bln)
{
	if(bln->state == 0)
	{
		bln->state = 1;
		return 0;
	}
	if(bln->state == 1)
	{
		bln->state = 0;
		return 0;
	}
	return -1;
}