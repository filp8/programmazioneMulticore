all: main run

run:
	mpirun ./build/main

main: main.c
	mpicc -Wall -Wextra -ggdb -I ./include -o ./build/main main.c