CC=mpiexec
LIB=-lm

OBJ=reconstruct.o pgmio.o
FILES=reconstruct.c pgmio.c
SERIAL=serial.o pgmio.o

.PHONY : serial
serial : $(SERIAL)
	$(CC) $^ -o $@
	
.PHONY : explicit_c
explicit_c: $(OBJ)
	$(CC) $(FILES) $(LIB) -o $@


.c.o: 
	$(CC) -c $<

.PHONY : reconstruct
reconstruct: $(OBJ)
	$(CC) $(OBJ) $(LIB) -o $@
	
.PHONY : clean	
clean:
	rm *.o 
	rm reconstruct
	rm serial 
	rm *~
	
	
