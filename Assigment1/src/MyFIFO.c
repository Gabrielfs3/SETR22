#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 50

int fifo[MAX_SIZE];
int pos;
   
int count=0;

void MyFIFOInit()
{
    do
    {
       fifo[count]=0;
       count++;
    } while (count<MAX_SIZE);
   printf("%d\n",fifo[10]);   
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