#include <stdio.h>
#include <stdlib.h>
#include "vinit.h"
int main(){
    int size;
    printf("Escolha um tamanho de array:");
    scanf("%d",&size);
    int vect[size];
    int vect1 = vinit(vect,size);
    /*for (int i=0; i<10;i++)
    {
        printf("%d\n", vect[i]);
    }*/
    double avg;
    avg = vAvg(vect1,size);
    printf("avg = %f\n", avg);
    return 0;
}