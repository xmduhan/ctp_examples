all : *.cpp
	g++ -pthread *.cpp /etc/ctp/lib/*.so ../common/*.o -o instrument_available


clean :
	rm -f *.o rm *.out *.con instrument_available
