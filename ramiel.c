#include "lexer.c"

int main(int argc, char *args[]) {
	if (!args[1]) {
		printf("Needs more arguments!\n");
		return 1;
	}

	lex_state.input = fopen(args[1], "r");	
	if (!lex_state.input) {
		printf("Invalid file\n");
		return 1;
	}

	if (make_tokens()) {
		printf("Error generating tokens. Exiting...\n");
		return 1;
	}

	if (verify_syntax()) {
		printf("File contains syntax errors. Exiting...\n");
		return 1;
	}

	return 0;
}