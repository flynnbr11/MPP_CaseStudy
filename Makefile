CC=mpicc
LIB=-lm

OBJ=reconstruct.o pgmio.o
FILES=reconstruct.c pgmio.c
SERIAL=serial.o pgmio.o


.c.o: 
	$(CC) -c $<

reconstruct: $(OBJ)
	$(CC) $(OBJ) $(LIB) -o $@
	


.PHONY : clean	
clean:
	rm *.o 
	rm reconstruct
	rm serial 
	rm *~
	
	
