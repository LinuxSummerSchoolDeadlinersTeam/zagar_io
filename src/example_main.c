#include "../include/example.h"

int main()
{
	struct boolean *mybool;
	mybool = malloc(EXAMPLE_BOOLEAN_ARRAY_SIZE * sizeof(struct boolean));

	mybool->state = 0;
	printf("%d\n", mybool->state);

	if(reverse_boolean(mybool) == -1)
		printf("ERROR");

	printf("%d\n", mybool->state);

	free(mybool);
	exit(0);
}