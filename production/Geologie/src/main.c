// C Program for the above approach
#include <unistd.h>
#include <stdio.h>
#include "tools.h"
#include "ReceptionistLOG/receptionistLOG.h"
#include "Receivers/receivers.h"

int main()
{
    ReceptionistLOG_new();
    Receivers_start(NULL);
    ReceptionistLOG_start();
    Receivers_stop(NULL);

    return 0;
}
