all: utils run

run:
	g++ -o part2 part2.c adcUtilsOpenCL.o printInfo.o error.o -lOpenCL

utils:
	g++ -c adcUtilsOpenCL.c -lOpenCL
	g++ -c printInfo.c -lOpenCL
	g++ -c error.c -lOpenCL

clean:
	rm -rf *.o
