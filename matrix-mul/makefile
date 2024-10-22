

all: serial run-serial


serial: serial.c
	mkdir -p build
	gcc -O3 -Wall -Wextra -I ./include -o ./build/serial serial.c

run-serial:
	./build/serial 3

main: main.c
	mpicc -O3 -Wall -Wextra -I ./include -o ./build/main main.c

debug:
	mpicc -Og -Wall -Wextra -ggdb -I ./include -o ./build/main main.c -DDEBUG