#include <stdio.h>
#include <stdlib.h>

void vAvg(int vect[], int size)
{

static double avg;
static int i;
static int sum=0;

for(i = 0; i < size; i++)
{
    sum = vect[i]+sum;
}

avg = sum/size;

printf("Avg of all values is: %.2f \n",avg);

}