#include <stdio.h>
#include <stdlib.h>
#include "MyFIFO.h"

int main()
{
    MyFIFOInit();
    
    for (int i = 0; i <= 8; i++)
    {
        if(i < 4)
        {
            MyFIFOInsert();
        }
        else if(i == 4)
        {
            MyFIFORemove();
        }
        else if (i > 4 && i < 7)
        {
            MyFIFOInsert();
            MyFIFORemove();
        }
        else
        {
            MyFIFORemove();
        }        
    }
    
    MyFIFOPeep();

    return 0;
}
