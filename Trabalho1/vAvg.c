/**
 * @file vAvg.c
 * @author Gabriel Silva (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-03-21
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <stdlib.h>
/**
 * @brief Função para calcular a media dos valores do array
 * 
 * @param vect array
 * @param size tamanho do array
 */
void vAvg(int vect[], int size)
{

static double avg;
static int i;
static int sum=0;

for(i = 0; i < size; i++)
{
    sum = vect[i]+sum;
}

avg = sum/size;

printf("Avg of all values is: %.2f \n",avg);

}