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