all: test

test:
	gcc -o test t/main.c
	./test
	rm -f test
