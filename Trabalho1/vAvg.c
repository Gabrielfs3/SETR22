#include <stdio.h>
#include <stdlib.h>

double vAvg(int vect[], int size)
{

static double avg;
static int i;
static int sum;

for(i = 0; i <= size; i++)
{
    sum += vect[i];
}
avg = sum/size;

return avg;
}