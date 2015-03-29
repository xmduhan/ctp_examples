IDIR =../include
LIBS =../lib/*.o /etc/ctp/lib/*.so
CC=g++
CFLAGS=-pthread -I$(IDIR) $(LIBS)

all : *.cpp
	$(CC) *.cpp $(CFLAGS) -o OrderInsert


clean :
	rm -f *.o rm *.out *.con *.orig OrderInsert