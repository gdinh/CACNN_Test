all:
	mkdir -p build
	mkdir -p lib
	g++ src/cacnn.c -O3 -Iinclude -c -o build/cacnn.o -g
	g++ src/convolve.c -O3 -Iinclude -c -o build/convolve.o -g
	g++ src/carma.c -O3 -Iinclude -c -o build/carma.o -g
	ar rcs lib/libcacnn.a build/cacnn.o
	ar rcs lib/libconvolve.a build/convolve.o
	ar rcs lib/libcarma.a build/carma.o
	g++ src/main.c lib/* -Iinclude -O3 -o measure -g -fpermissive
	#gcc src/main.c lib/* -Iinclude -I/home/edyounis/Workspace/applications/libpfc/include -lpfc -lm -O0 -o measure -g

clean:
	rm -rf build
	rm -rf lib
	rm *.out
	rm measure