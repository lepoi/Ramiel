#include "lexer.c"

int main(int argc, char *args[]) {
	if (run_lexer(args)) {
		printf("Error generating tokens. Exiting...\n");
		return 1;
	}

	return 0;
}