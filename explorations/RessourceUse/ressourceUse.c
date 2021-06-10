#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void getCpu() {
    int cpuUse;

   // stores each word in status file
    char buffer[1024] = "";

    // linux file contains cpu info info
    FILE* file = fopen("/proc/stat", "r");

    // read the entire file
    while (fscanf(file, " %1023s", buffer) == 1) {
        if (strcmp(buffer, "cpu") == 0) {
            fscanf(file, " %d", &cpuUse);
            break;
        }

    }
    fclose(file);

    printf("CPU use: %d\n", cpuUse);
}

void getMemory() {
    int MemTotal;
    int MemFree;
    int notAllFound = 0;
    char buffer[1024] = "";

        // linux file contains memory info info
    FILE* file = fopen("/proc/meminfo", "r");

    // read the entire file
    while (fscanf(file, " %1023s", buffer) == 1 && notAllFound < 2) {

        if (strcmp(buffer, "MemTotal:") == 0) {
            fscanf(file, "%d", &MemTotal);
            notAllFound++;
        }
        if (strcmp(buffer, "MemFree:") == 0) {
            fscanf(file, "%d", &MemFree);
            notAllFound++;
        }
    }
    fclose(file);

    printf("Memory use: %d kB\n", MemTotal - MemFree);
}

int main() {
    while (1) {
        getCpu();
        getMemory();
        sleep(2);
    }

    return 0;
}
