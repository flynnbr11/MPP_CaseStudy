#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pgmio.h"
#include "mpi.h"

int main(int argc, char *argv[])
{

	MPI_Init(NULL, NULL);
	double begin_timer = MPI_Wtime();
	double end_timer, time_taken;
	int P, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &P);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//printf("There are %d processors, this is rank %d \n", P, rank);

	int M = 192;
	int N = 128;
	float masterbuf[M][N];

	if(rank==0){
		pgmread("edge192x128.pgm", masterbuf, M, N);
	 //  printf("In first rank 0 if loop \n");
	}

	int i,j,t,T;
	T=atoi(argv[1]);
	int Mp, Np;
	Mp=M/P;
	Np=N;
	int product = Mp*Np;
	float buf[Mp][Np];
	float old[Mp+2][Np+2];
	float edge[Mp+2][Np+2];
	float new[Mp+2][Np+2];

	MPI_Scatter(masterbuf, product, MPI_FLOAT, buf, product, MPI_FLOAT, 0, MPI_COMM_WORLD);

	for(i=1; i<=Mp;i++){
		for(j=1; j<=Np;j++){
		   edge[i][j] = buf[i-1][j-1];
		}
	}



	for(i=0; i<Mp+2;i++){
		for(j=0; j<Np+2;j++){
		   old[i][j]=255;
		}
	}


	for(t=0; t<T;t++){
		for(i=0; i<Mp;i++){
		   for(j=0;j<Np;j++){
		      new[i][j]= ((old[i-1][j] + old[i+1][j] +old[i][j-1] + \
		       old[i][j+1]- edge[i][j])/4.0) ;
		       }
		 }
		 

		for(i=1; i<Mp+2;i++){
		   for(j=1; j<Np+2;j++){
		      old[i][j]=new[i][j];
		   }
		}    
	}

		for(i=1; i<=Mp;i++){
		   for(j=1; j<=Np;j++){
		      buf[i-1][j-1]=old[i][j];
		   }
		}    

	MPI_Gather(buf, product, MPI_FLOAT, masterbuf, product, MPI_FLOAT, 0, MPI_COMM_WORLD);

	if(rank==0){
		pgmwrite(argv[2], masterbuf,M, N);

		end_timer = MPI_Wtime();
		time_taken = end_timer - begin_timer;
		printf("Time taken: %f \n", time_taken);
	}

	MPI_Finalize();

	return 0;
}
