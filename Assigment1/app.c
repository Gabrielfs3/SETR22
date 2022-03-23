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
    MyFIFOPeep();
    return 0;
}
