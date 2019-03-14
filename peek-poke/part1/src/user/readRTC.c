#include <stdlib.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    printf("Starting program\n");
    char * p = 0x400240FF; 
    int i;
    for (i = 0; i < 4; i++ ){
        printf("Value of byte %d: %d\n", i ,p[i]);
    }
    printf("Finished.\n");
    return 0;
}
