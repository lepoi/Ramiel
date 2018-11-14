struct state {
	FILE *input;
	struct token_info *current;
	struct token_info *list;
	struct tree_node *tree;
	unsigned short errors;
	unsigned short warnings;
	unsigned short line;
	unsigned short column;
};

struct state lex_state = (struct state) { NULL, NULL, NULL, NULL, 0, 0, 1, 0 };

void append_token(struct token_info *n_info) {
	if (lex_state.list == NULL) {
		lex_state.list= n_info;
		return;
	}
	struct token_info *pointer = lex_state.list;
	while (pointer->next != NULL)
		pointer = pointer->next;

	pointer->next = n_info;
}