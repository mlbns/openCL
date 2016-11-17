all: utils run

run:
	g++ -o lala part1.c adcUtilsOpenCL.o -lOpenCL

utils:
	g++ -c adcUtilsOpenCL.c -lOpenCL

clean:
	rm -rf *.o