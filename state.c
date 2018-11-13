struct state {
	FILE *input;
	FILE *output;
	struct token_info *entry;
	unsigned short errors;
	unsigned short warnings;
	unsigned short line;
	unsigned short column;
};

struct state lex_state = (struct state) { NULL, NULL, NULL, 0, 0, 1, 1 };

void append_token(struct token_info *n_info) {
	if (lex_state.entry == NULL) {
		lex_state.entry= n_info;
		return;
	}
	struct token_info *pointer = lex_state.entry;
	while (pointer->next != NULL)
		pointer = pointer->next;

	pointer->next = n_info;
}