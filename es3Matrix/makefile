all: parallel run-parallel

run-parallel:
	mpirun ./build/parallel 4 3 3

parallel: parallel.c
	mkdir -p build
	mpicc -O3 -Wall -Wextra -I ./include -o ./build/parallel parallel.c

debug:
	mpicc -Og -Wall -Wextra -ggdb -I ./include -o ./build/parallel parallel.c -DDEBUG

run-sequential:
	./build/sequential 4 3 3

sequential: sequential.c
	mkdir -p build
	gcc -O3 -Wall -Wextra -I ./include -o ./build/sequential sequential.c