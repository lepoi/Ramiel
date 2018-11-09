all:
	gcc ramiel.c -o ramiel -Wall -Wextra
lexer:
	gcc lexer.c -o lexer
	./lexer test.ram