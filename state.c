struct state {
	FILE *input;
	FILE *output;
	unsigned short errors;
	unsigned short warnings;
	unsigned short line;
	unsigned short column;
};

struct state new_state() {
	struct state new_state;
	new_state.input = NULL;
	new_state.errors = 0;
	new_state.warnings = 0;
	new_state.line = 1;
	new_state.column = 1;

	return new_state;
}