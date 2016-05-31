#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *stat_file = fopen("/proc/self/stat", "r");
    char ppid[16];

    fscanf(stat_file, "%*s %*s %*s %s", ppid);
    puts(ppid);

    return 0;
}
