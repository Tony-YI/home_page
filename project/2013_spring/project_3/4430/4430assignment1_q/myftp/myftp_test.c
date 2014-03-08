#include <stdio.h>
#include "myftp.h"

int main(void)
{
	struct message_s item;
	printf( "%d\n", sizeof(item) );
	return 0;
}
