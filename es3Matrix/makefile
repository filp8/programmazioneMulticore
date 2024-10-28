all: parallel run-parallel

run-parallel:
	mpirun --oversubscribe -n 4 ./build/parallel 4 4 4 

parallel: parallel.c
	mkdir -p build
	mpicc -O3 -Wall -Wextra -I ./include -o ./build/parallel parallel.c

debug:
	mpicc -Og -Wall -Wextra -ggdb -I ./include -o ./build/parallel parallel.c -DDEBUG

run-sequential:
	./build/sequential 4 4 4

sequential: sequential.c
	mkdir -p build
	gcc -O3 -Wall -Wextra -I ./include -o ./build/sequential sequential.c