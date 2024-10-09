all: main run

run:
	mpirun -n 4 ./build/main

main: main.c
	mpicc -Wall -Wextra -ggdb -I ./include/ -o ./build/main main.c
