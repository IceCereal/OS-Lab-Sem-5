/*
	2019-July-29

	Create an array (user-defined size) of random integers [0-1000]
		(a) No repetitions
		(b) No range dominate

*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <time.h>

int main(int argc, char *argv[]){

	// argv[1] = size of the array

	char *p;
	errno = 0;
	int array_size = strtol(argv[1], &p, 10);

	// Array Size - Convert
	if (errno != 0 || *p != '\0' || array_size > INT_MAX){
		printf("Input Incorrect - argv[1] - Array Size\n");
		exit(1);
	}

	// Create The Array
	int *array = (int *)malloc(array_size * sizeof(int));
	int *positions = (int *)malloc(array_size * sizeof(int));

	int i;

	// Positions = 1:array_size
	for (i = 0; i < array_size; ++i)
		positions[i] = i;

	srand(time(0));

	for (i = 0; i < array_size; ++i){
		int index = rand() % array_size;

		int flag = 1;
		while (flag){
			if (positions[index] == -1){
				++index;
				if ( index > (array_size-1) ){
					index = 0;
				}
			}
			else{
				array[i] = positions[index];
				positions[index] = -1;
				flag = 0;
			}
		}
	}

	for (i = 0; i < array_size; ++i)
		printf("%d\n", array[i]);

	free(array);
	free(positions);

	return 0;
}
