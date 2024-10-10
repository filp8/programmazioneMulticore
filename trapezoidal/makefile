all: main run

run:
	mpirun -n 4 ./build/main 0 1 10

main: main.c
	mpicc -O3 -Wall -Wextra -ggdb -I ./include -o ./build/main main.c -lm