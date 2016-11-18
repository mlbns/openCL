all: utils run

run:
	g++ -o lala part1.c adcUtilsOpenCL.o error.o -lOpenCL

utils:
	g++ -c adcUtilsOpenCL.c -lOpenCL
	g++ -c error.c -lOpenCL

clean:
	rm -rf *.o