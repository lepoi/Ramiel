#include "lexer.c"
#include "parser.c"

int main(int argc, char *args[]) {
	if (argc < 2) {
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

	lex_state.output = fopen("out.wal", "wb+");
	if (!lex_state.output) {
		printf("Error opening \"out.wal\"\n");
		return 1;
	}
	lex_state.stack = new_stack();
	lex_state.variables = new_ht(128);

	if (analyze()) {
		printf("File contains syntax errors. Exiting...\n");
		return 1;
	}

	fclose(lex_state.input);
	fclose(lex_state.output);

	return 0;
}