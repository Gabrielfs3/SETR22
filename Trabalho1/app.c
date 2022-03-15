#include <stdio.h>
#include <stdlib.h>
#include "vinit.h"
int main(){
    int size;
    printf("Escolha um tamanho de array:");
    scanf("%d",&size);
    int vect[size];
    vinit(vect,size);
    /*for (int i=0; i<10;i++)
    {
        printf("%d\n", vect[i]);
    }*/
    return 0;
}