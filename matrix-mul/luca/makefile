

#all: parallel run-parallel
all: serial run-serial

serial-debug: serial.c
	mkdir -p build
	gcc -Og -ggdb -Wall -Wextra -I ./include -o ./build/serial serial.c -DDEBUG

serial: serial.c
	mkdir -p build
	gcc -O3 -ggdb -Wall -Wextra -I ./include -o ./build/serial serial.c

run-serial:
	./build/serial 10

parallel: main.c
	mpicc -O3 -Wall -Wextra -I ./include -o ./build/parallel main.c

parallel-debug:
	mpicc -Og -Wall -Wextra -ggdb -I ./include -o ./build/parallel main.c -DDEBUG

run-parallel:
	mpirun -n 4 ./build/parallel 200