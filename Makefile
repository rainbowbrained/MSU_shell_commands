CC=gcc --coverage -O2 -ftrapv -fsanitize=address,undefined -Wall -Werror -Wformat-security -Wignored-qualifiers -Winit-self -Wswitch-default -Wfloat-equal -Wshadow -Wpointer-arith -Wtype-limits -Wempty-body -Wlogical-op -Wstrict-prototypes -Wold-style-declaration -Wold-style-definition -Wmissing-parameter-type -Wmissing-field-initializers -Wnested-externs -Wno-pointer-sign -std=gnu11 -lm

all: p04.o 
	$(CC) p04.o -o p04

p04.o: p04.c
	$(CC) -O0 -g -c p04.c

p04:
	./p04

clean:
	rm -rf *.o *.gcda *.gcno p04
