/**
 * @file vinit.c
 * @author Luis Malarmey (you@domain.com)
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
 * @brief Função que preenche o array
 * 
 * @param vect Array de inteiros
 * @param size Tamanho do array
 * @return vect[size] 
 */
int vinit (int vect[],int size)
{
    static int cnt = 1;
    for(int i=0; i<size; i++)
    {
        vect[i]=cnt;
        cnt++;
    }
return vect[size];
}