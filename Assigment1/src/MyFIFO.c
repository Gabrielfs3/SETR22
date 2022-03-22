#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 50



int fifo[MAX_SIZE];
int pos;
int first=0;
int count=0;

void MyFIFOInit()
{
    do
    {
       fifo[count]=0;
       count++;
    } while (count<MAX_SIZE);
}

void MyFIFOInsert()
{
    int i;
    for (i = 0; i < MAX_SIZE; i++)
    {
        if(fifo[i]==0)
        {
            fifo[i] = 1;
            pos = i;
            break;
        }
    }
}

void MyFIFORemove()
{  
    for(int i = 0; i < MAX_SIZE; i++)
    {
        if(fifo[i] != 0)
        {
            fifo[first] = 0;
            first++;
            break;
        }
    }
}

void MyFIFOPeep()
{
    printf("oldest position: %d\n",first);
    printf("oldest element: %d\n",fifo[first]);
}