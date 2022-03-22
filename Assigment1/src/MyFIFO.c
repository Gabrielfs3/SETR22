#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 50

int fifo[MAX_SIZE];
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

}

void MyFIFORemove()
{

}

int MyFIFOPeep()
{

}

int MyFIFOSize()
{

}