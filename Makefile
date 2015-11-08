CC=mpicc
LIB=-lm

OBJ=reconstruct.o pgmio.o

reconstruct: $(OBJ)
	$(CC) -o $(OBJ) $(LIB)
	
.c.o:
	$(CC) -c $<
	
clean:
	rm *.o reconstruct
	
	
	
