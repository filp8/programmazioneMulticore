all: main run

run:
	mpirun ./build/main

main: main.c
	mpicc -O3 -Wall -Wextra -I ./include -o ./build/main main.c

debug:
	mpicc -Og -Wall -Wextra -ggdb -I ./include -o ./build/main main.c -DDEBUG