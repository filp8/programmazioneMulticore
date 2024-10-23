

all: serial-debug run-serial


serial-debug: serial.c
	mkdir -p build
	gcc -Og -ggdb -Wall -Wextra -I ./include -o ./build/serial serial.c -DDEBUG

serial: serial.c
	mkdir -p build
	gcc -O3 -ggdb -Wall -Wextra -I ./include -o ./build/serial serial.c

run-serial:
	./build/serial 3

main: main.c
	mpicc -O3 -Wall -Wextra -I ./include -o ./build/main main.c

debug:
	mpicc -Og -Wall -Wextra -ggdb -I ./include -o ./build/main main.c -DDEBUG