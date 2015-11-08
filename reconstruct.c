#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pgmio.h"


int main()
{
int M = 192;
int N = 128;
float image[M][N];

pgmread("edge192x128.pgm", image, M, N);
pgmwrite("new.png", image, M, N);

}
