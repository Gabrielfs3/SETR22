#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 50

int fifo[MAX_SIZE];
int pos;
int first=0;
int count=0;
int rear=0;

void MyFIFOInit()
{
    do
    {
       fifo[count]=0;
       count++;
    } while (count<MAX_SIZE);
    printf("Fifo just started.\n");
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
    printf("You just inserted a 1 on fifo position: %d\n",pos);
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

void MyFIFORemove()
{  
    
    for(int i = 0; i < MAX_SIZE; i++)
    {
        if(fifo[i] != 0 && first==i)
        {
            printf("You just removed the position %d of the fifo.\n",first);
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
