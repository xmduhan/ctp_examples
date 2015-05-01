IDIR =../include
LIBS =../lib/*.o ../lib/*.so
CC=g++
CFLAGS=-pthread -I$(IDIR) $(LIBS)

all : *.cpp
	$(CC) *.cpp $(CFLAGS) -o execute 


clean :
	rm -f *.o rm *.out *.con *.orig execute 