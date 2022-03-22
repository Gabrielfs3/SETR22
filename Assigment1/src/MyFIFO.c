#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 50



int fifo[MAX_SIZE];
int pos;
int first;
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
    printf("%d",fifo[pos]);
}

void MyFIFOSize()
{
    int tamanho=0;
    for(int i=0;i<MAX_SIZE;i++)
    {
        if(fifo[i]!=0)
            tamanho++;
    }
    printf("The size of the FIFO is %d\n", tamanho);
}



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
