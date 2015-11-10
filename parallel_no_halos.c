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
	int rank_left = rank -1;
	int rank_right = rank + 1;
	int M = 192;
	int N = 128;
	float masterbuf[M][N];

	if(rank==0){
		pgmread("edge192x128.pgm", masterbuf, M, N);
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
//Distribute the given image among the processors
	MPI_Scatter(masterbuf, product, MPI_FLOAT, buf, product, MPI_FLOAT, 0, MPI_COMM_WORLD);
//Initialise edge array on each processor
	for(i=1; i<=Mp;i++){
		for(j=1; j<=Np;j++){
		   edge[i][j] = buf[i-1][j-1];
		}
	}
// Initialise old array to all white values
	for(i=0; i<Mp+2;i++){
		for(j=0; j<Np+2;j++){
		   old[i][j]=255;
		}
	}

// Begin time loop
	for(t=0; t<T;t++){
		for(i=0; i<Mp;i++){
		   for(j=0;j<Np;j++){
		      new[i][j]= ((old[i-1][j] + old[i+1][j] +old[i][j-1] + \
		       old[i][j+1]- edge[i][j])/4.0) ; //Update "new" array
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
		      buf[i-1][j-1]=old[i][j]; //Copy old array to buf
		   }
		}    
//Regather distributed arrays into original array on rank 0
	MPI_Gather(buf, product, MPI_FLOAT, masterbuf, product, MPI_FLOAT, 0, MPI_COMM_WORLD);
//Print the updated image to a file named as the second command line argument
	if(rank==0){
		pgmwrite(argv[2], masterbuf,M, N);

		end_timer = MPI_Wtime();
		time_taken = end_timer - begin_timer;
		printf("Time taken: %f \n", time_taken);
	}

	MPI_Finalize();

	return 0;
}
