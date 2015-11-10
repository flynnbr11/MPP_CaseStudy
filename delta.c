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
	int P, rank; //related to MPI 
	MPI_Request request[4];
	int left_neighbour, right_neighbour;
	int M = 192;
	int N = 128;
	int i,j,t,T; // loop variables
	int Mp, Np; // subset of M, N depending on number of processors
	MPI_Comm_size(MPI_COMM_WORLD, &P); // assign P as total num processors
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // rank of current processor = rank

	Mp=M/P;
	Np=N;	int product = Mp*Np; //size of each chunk given to each processor
	float masterbuf[M][N];
	float buf[Mp][Np];
	float old[Mp+2][Np+2];
	float edge[Mp+2][Np+2];
	float new[Mp+2][Np+2];
   float delta =1.0;
   float threshold = 0.1;
   float current_max = 0.1;
   float max_change = 2.0;
	T=atoi(argv[1]); // define number of iterations on command line, for script
//	T=100;
	

	if(rank==0){
//		pgmread(argv[3], masterbuf, M, N);
		pgmread("edge192x128.pgm", masterbuf, M, N);
	}

	
	left_neighbour = rank - 1;
	right_neighbour = rank + 1;
	if(left_neighbour<0) {
		left_neighbour = MPI_PROC_NULL;
	}
	if(right_neighbour > P-1) {
		right_neighbour = MPI_PROC_NULL;
	}
	
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

		// Begin time loop, if previous iteration has no change larger than the threshold. 
   for(t=0; t<T;t++){
//   	while(max_change > threshold) { 
         current_max=0;
		   //halo swaps
	      printf("on iteration %d  and max_change = %f \n", t, max_change);
		   MPI_Isend(&old[Mp][1], N, MPI_FLOAT, right_neighbour, 0, MPI_COMM_WORLD, &request[0]);
		   MPI_Irecv(&old[0][1], N, MPI_FLOAT, left_neighbour, 0, MPI_COMM_WORLD, &request[1]);
		   MPI_Isend(&old[1][1], N, MPI_FLOAT, left_neighbour, 0, MPI_COMM_WORLD, &request[2]);
		   MPI_Irecv(&old[Mp+1][1], N, MPI_FLOAT, right_neighbour, 0, MPI_COMM_WORLD, &request[3]);

		   MPI_Wait(&request[0], MPI_STATUS_IGNORE);	
		   MPI_Wait(&request[1], MPI_STATUS_IGNORE);	
		   MPI_Wait(&request[2], MPI_STATUS_IGNORE);	
		   MPI_Wait(&request[3], MPI_STATUS_IGNORE);	
	
		   for(i=1; i<=Mp;i++){
		      for(j=1;j<=Np;j++){
		         new[i][j]= ((old[i-1][j] + old[i+1][j] +old[i][j-1] + \
		          old[i][j+1] - edge[i][j])/4.0) ; //Update "new" array
		          }
		    }


		   for(i=1; i<=Mp;i++){
		      for(j=1; j<=Np;j++){
               delta = new[i][j] - old[i][j];
               
               if(delta > current_max) current_max = delta;
		         old[i][j]=new[i][j];
		      }
		   }   
		
		MPI_Reduce(&current_max, &max_change, 1, MPI_FLOAT, MPI_MAX, rank, MPI_COMM_WORLD);    
	   if(max_change < threshold) {
	      printf("Ran %d iterations. \n", t);
	      break;
      }
	//  }
   }
		for(i=1; i<=Mp;i++){
		   for(j=1; j<=Np;j++){
		      buf[i-1][j-1]=old[i][j]; //Copy old array to buf
		   }
		}    
		//Regather distributed arrays into original array on rank 0
	MPI_Gather(buf, product, MPI_FLOAT, masterbuf, product, MPI_FLOAT, 0, MPI_COMM_WORLD);
		//Print the updated image to a file named to the second command line argument
	if(rank==0){
		pgmwrite(argv[2], masterbuf,M, N);

		end_timer = MPI_Wtime();
		time_taken = end_timer - begin_timer;
		printf("Time taken: %f seconds \n", time_taken);
	}

	MPI_Finalize();

	return 0;
}
