#include <stdio.h>
#include <stdlib.h>

int vinit (int vect[],int size)
{
    static int cnt = 1;
    for(int i=0; i<size; i++)
    {
        vect[i]=cnt;
        cnt++;
    }
return vect[size];
}