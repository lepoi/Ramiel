all:
	gcc ramiel.c -o ramiel -Wall -Wextra

test:
	gcc ramiel.c -o ramiel -Wall -Wextra
	./ramiel test.ram