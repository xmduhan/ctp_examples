IDIR =../include
LIBS =../lib/*.o ../lib/*.so
CC=g++
CFLAGS=-pthread -I$(IDIR) $(LIBS)

all : *.cpp
	$(CC) *.cpp $(CFLAGS) -o execute {# {{ apiName }} #}


clean :
	rm -f *.o rm *.out *.con *.orig execute {# {{ apiName }} #}
