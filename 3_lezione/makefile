main: main.c
	mpicc -ggdb -Wall -Wextra -I ./include/ main.c -o ./build/main
run:
	mpirun --oversubscribe -n 4 ./build/main
all: main run