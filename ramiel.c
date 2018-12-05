#include "lexer.c"
#include "parser.c"

int main(int argc, char *args[]) {
	if (argc < 2) {
		printf("Needs more arguments!\n");
		exit(1);
	}

	lex_state.input = fopen(args[1], "r");	
	if (!lex_state.input) {
		printf("Invalid file\n");
		exit(1);
	}

	if (make_tokens()) {
		printf("Error generating tokens. Exiting...\n");
		fclose(lex_state.input);
		exit(1);
	}

	lex_state.output = fopen("out.we", "wb+");
	if (!lex_state.output) {
		printf("Error opening \"out.we\"\n");
		return 1;
	}
	lex_state.stack = new_stack();
	lex_state.variables = new_ht(128);

	char error = analyze();
	fclose(lex_state.output);

	if (error) {
		printf("File contains errors. Removing out.we...\n");
		if (remove("out.we"))
			printf("Error removing out.we\n");
	}
	else
		printf("Parsed file successfully!\n");
	printf("Exiting...\n");

	return error;
}