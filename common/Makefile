
all : compile copy

compile : *.cpp 
	g++ -c *.cpp  

copy : *.o *.h
	cp *.o ../lib
	cp *.h ../include

	

clean :
	rm -f *.o *.out 
