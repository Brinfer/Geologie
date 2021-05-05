// C Program for the above approach
#include <unistd.h>
#include <stdio.h>
#include "tools.h"


int main()
{
    int i, n=2;
    char str[50];

    PRINT("HELLO\n");
    PRINT("HELLO%d%s", 2, "\n");

    TRACE("HELLO%d%s", 5, "\n");

    STOP_ON_ERROR(1 != 0);

    return 0;
}
