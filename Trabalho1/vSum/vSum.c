#include <stdio.h>
#include <stdlib.h>
//#include <vinit.h>



int main(void)
{

    int vect[] = {1,2,3,4,5,6,7,7,8,9,10};


    int sum = 0;
    int j, a;

    for (j=0; j < 10; j++)
    
    {
        a = vect[j];  
        sum = sum + a; 
        j++;
    }

    printf("Sum of all values is: %d \n", sum);

    return 0;
}
