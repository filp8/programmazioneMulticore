main: main.c
	mpicc -ggdb -Wall -Wextra -I ./include/ main.c -o ./build/main
main_run:
	mpirun --oversubscribe -n 4 ./build/main

iter_sum: iter_sum.c
	gcc -ggdb -Wall -Wextra -I ./include/ -o ./build/iter_sum iter_sum.c
run_iter_sum:
	./build/iter_sum

mpi_sum: mpi_sum.c
	mpicc -ggdb -Wall -Wextra -I ./include/ -o ./build/mpi_sum mpi_sum.c
run_mpi_sum:
	mpirun --oversubscribe -n 4 ./build/mpi_sum

log_sum: log_sum.c
	mpicc -ggdb -Wall -Wextra -I ./include/ -o ./build/log_sum log_sum.c -lm
run_log_sum:
	mpirun --oversubscribe -n 4 ./build/log_sum

all: main run