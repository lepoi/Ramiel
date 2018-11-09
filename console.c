#define C_R "\e[1;31m"
#define C_G "\e[1;32m"
#define C_B "\e[1;34m"
#define C_Y "\e[1;33m"
#define C_RST "\e[0m"

#include <stdio.h>

void error_m(char *message, struct state *lex_state, ushort line, ushort column) {
	printf(C_R"*** ERROR ***"C_B" line %u, column %u\n"C_RST, line, column);
	printf("%s\n\n", message);
	lex_state->errors++;
}

void warning_m(char *message, struct state *lex_state, ushort line, ushort column) {
	printf(C_Y"* WARNING *"C_B" line %u, column %u\n"C_RST, line, column);
	printf("%s\n\n", message);
	lex_state->warnings++;
}

void standard_m(char *message) {
	printf(C_B"%s\n\n"C_RST, message);
}