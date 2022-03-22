#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 50

int fifo[MAX_SIZE];
int pos;

void MyFIFOInit()
{
    
}

void MyFIFOInsert()
{
    for (int i = 0; i < MAX_SIZE; i++)
    {
        if(fifo[i]=0)
        {
            fifo[i] = 1;
            pos = i;
            break;
        }
    }
    printf("%d",fifo[pos]);
}