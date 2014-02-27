all: test

test:
	gcc -g -o test t/main.c
	./test
	rm -f ./test
