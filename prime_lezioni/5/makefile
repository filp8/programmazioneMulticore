all: main trapezoidal

run-main:
	mpirun -n 4 ./build/main

main: main.c
	mpicc -Wall -Wextra -ggdb -o ./build/main -I ./include main.c

trapezoidal: trapezoidal_rule.c
	mpicc -Wall -Wextra -ggdb -o ./build/trapezoidal_rule -I ./include trapezoidal_rule.c

run-trapezoidal:
	mpirun -n 4 ./build/trapezoidal_rule