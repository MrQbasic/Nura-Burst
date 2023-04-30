#include <stdio.h>
#include <stdlib.h>

#define path "output.csv"

void cvs_add_line(float a, float b, float c){
    FILE* f = fopen(path, "a");
    if(f == NULL) exit(1);
    fprintf(f, "%f\t%f\t%f\r\n", a, b, c);
    fclose(f);
}