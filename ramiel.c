#include "console.c"
#include "state.c"
#include "lexer.c"

int main() {
	struct state comp_state
	unsigned int e_count = 0, w_count = 0;
	error_m("xD", &e_count);
	error_m("xD", &e_count);
	error_m("xD", &e_count);
	error_m("xD", &e_count);
	error_m("xD", &e_count);
	error_m("xD", &e_count);
	error_m("xD", &e_count);
	warning_m("xD", &w_count);
	warning_m("xD", &w_count);
	warning_m("xD", &w_count);
	warning_m("xD", &w_count);
	printf("e: %u, w: %u\n", e_count, w_count);
	return 0;
}