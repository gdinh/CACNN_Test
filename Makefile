all:
	mkdir -p build
	mkdir -p lib
	gcc src/cacnn.c -O1 -Iinclude -c -o build/cacnn.o -g
	gcc src/convolve.c -O1 -Iinclude -c -o build/convolve.o -g
	gcc src/carma.c -O1 -Iinclude -c -o build/carma.o -g
	ar rcs lib/libcacnn.a build/cacnn.o
	ar rcs lib/libconvolve.a build/convolve.o
	ar rcs lib/libcarma.a build/carma.o
	gcc src/main.c lib/* -Iinclude -I/home/edyounis/Workspace/applications/libpfc/include -lpfc -lm -O0 -o measure -g

clean:
	rm -rf build
	rm -rf lib
	rm *.out
	rm measure