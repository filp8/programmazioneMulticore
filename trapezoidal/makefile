all: main run

run:
	mpirun -n 4 ./build/main 0 1 4096

main: main.c
	mpicc -O3 -Wall -Wextra -I ./include -o ./build/main main.c -lm

debug:
	mpicc -Og -Wall -Wextra -ggdb -I ./include -o ./build/main main.c -lm -DDEBUG
	mpirun -n 4 ./build/main 0 1 4096