#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 50


void MyFIFORemove(int fifo[])
{  
    int pos = 1, size = 0;

    for(int i = 0; i < MAX_SIZE; i++)
    {
        if(fifo[i] != 0)
        {
            size += 1;
        }
    }

    for(int j = pos-1 ; j < size-1 ; j++)
    {
        fifo[j] = fifo[j+1];
    }

    size -= 1;

}


