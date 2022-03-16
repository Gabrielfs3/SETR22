#include <stdio.h>
#include <stdlib.h>
#include "vinit.h"
#include "vAvg.h"
#include "vSum.h"

int main(){

    int size;
    printf("Escolha um tamanho de array:");
    scanf("%d",&size);

    int vect[size];

    vinit(vect,size);

    vAvg(vect,size);

    vSum(vect,size);

    return 0;
}