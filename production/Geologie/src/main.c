/**
 * @file main.c
 *
 * @brief Fichier main du logiciel
 *
 * @version 2.0
 * @date 03-06-2021
 * @author GAUTIER Pierre-Louis
 * @copyright Geo-Boot
 * @license BSD 2-clauses
 */

#include "ManagerLOG/managerLOG.h"

/**
 * @brief Fonction main du logiciel
 *
 * @return int 0
 */
int main()
{
    ManagerLOG_startGEOLOGIE();

    ManagerLOG_stopGEOLOGIE();
    return 0;
}
