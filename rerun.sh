#!/bin/bash

mpicc reconstruct.c pgmio.c -o run
echo -n -e "  Number of processors \t  Timeloops \t  Time taken \n " > times.dat 

for num_cores in 1 2 4 8
   do
           for timeloops in 1 10 100 1000
              do
                  echo -n -e   ${num_cores} "\t " ${timeloops} "\t " >> times.dat 

                  mpiexec -n $num_cores  ./run $timeloops image_${num_cores}_cores_${timeloops}_iters.png >> times.dat
                  echo -n -e " \n " >> times.dat 
              done   
   done 
   
   
