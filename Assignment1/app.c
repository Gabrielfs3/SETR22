#include <stdio.h>
#include <stdlib.h>
#include "MyFIFO.h"

int main()
{
    MyFIFOInit();
    MyFIFOInsert();
    MyFIFOInsert();
    MyFIFOInsert();
    MyFIFOInsert();
    MyFIFORemove();
    MyFIFORemove();
    MyFIFORemove();
    MyFIFOPeep();
    MyFIFOSize();

    return 0;
}
