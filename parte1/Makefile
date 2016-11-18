all: utils run

run:
	g++ -o part1 part1.c adcUtilsOpenCL.o printInfo.o error.o -lOpenCL

utils:
	g++ -c adcUtilsOpenCL.c -lOpenCL
	g++ -c printInfo.c -lOpenCL
	g++ -c error.c -lOpenCL

clean:
	rm -rf *.o
