#include <stdio.h>
#include <stdlib.h>

void vSum(int vect[], int size)
{
    int sum = 0;
    int j, a;

    for (j=0; j < size; j++)
    {
        a = vect[j];  
        sum = sum + a;
    }

    printf("Sum of all values is: %d \n", sum);

}