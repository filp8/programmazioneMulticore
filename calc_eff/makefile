all: debug

run:
	./build/main -p ciao -s ciao

main: main.c
	mkdir -p build
	gcc -O3 -Wall -Wextra -I ./include -o ./build/main main.c

debug:
	gcc -Og -Wall -Wextra -ggdb -I ./include -o ./build/main main.c -DDEBUG