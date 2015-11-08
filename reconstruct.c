#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pgmio.h"


int main(int argc, char *argv[])
{
int M = 192;
int N = 128;
int i,j,t;
float buf[M][N];
float old[M+2][N+2];
float edge[M+2][N+2];
float new[M+2][N+2];
int limit;
char output_file;

//limit = argv[1];
//output_file= argv[2];

pgmread("edge192x128.pgm", buf, M, N);

for(i=1; i<=M;i++){
   for(j=1; j<=N;j++){
      edge[i][j] = buf[i-1][j-1];
   }
}
/*
for(i=0; i<M+2; i++){
   edge[i][0]=255;
   edge[i][N+2]=255;
   edge[0][i]=255;
   edge[N][i]=255;
}
*/
for(i=0; i<M+2;i++){
   for(j=0; j<N+2;j++){
      old[i][j]=255;
   }
}


for(t=0; t<1;t++){
   for(i=0; i<M;i++){
      for(j=0;j<N;j++){
         new[i][j]= ((old[i-1][j] + old[i+1][j] +old[i][j-1] + \
          old[i][j+1]- edge[i][j])/4.0) ;
          }
    }
    

   for(i=1; i<M+2;i++){
      for(j=1; j<N+2;j++){
         old[i][j]=new[i][j];
      }
   }    

}

   for(i=1; i<=M;i++){
      for(j=1; j<=N;j++){
         buf[i-1][j-1]=old[i][j];
      }
   }    

pgmwrite("output.png", edge,M+2, N+2);

return 0;
}
