#include "parser.h"

char analyze() {
	lex_state.current = lex_state.list;

	while (lex_state.current != NULL && lex_state.current->next != NULL) {
		if (!consume_token())
			lex_state.current = lex_state.current->next;
	}

	return 0;
}

void skip_tokens() {
	do {
		if (lex_state.current->tok->type == T_EOF)
			break;
		lex_state.current = lex_state.current->next;
	} while (lex_state.current->tok->type != S_SCLN && lex_state.current->tok->type != S_LCBR);

	lex_state.current = lex_state.current->next;
}

char consume_token() {
	char error = 0;
	char *m;

	switch (lex_state.current->tok->type) {
		case K_INT:
		case K_FLT:
		case K_CHR:
		case K_STR:
			error = expect_decl(lex_state.current->tok->type + 23);
			break;
		default:
			m = malloc(64);
			sprintf(m, ">> Found %.8s token\n", &token_names[lex_state.current->tok->type * 8]);
			warning_m(m, lex_state.current->line, lex_state.current->column);
			error = 1;
			break;
	}

	if (error)
		skip_tokens();

	return error;
}

char expect(enum token_type type, char required) {
	if (!lex_state.current->next)
		return 1;

	if (lex_state.current->next->tok->type == type) {
		lex_state.current = lex_state.current->next;
		return 0;
	}

	if (required) {
		char *m = malloc(64);
		sprintf(m, "Expected %.8s\nFound %.8s", &token_names[type * 8], &token_names[lex_state.current->next->tok->type * 8]);
		error_m(m, lex_state.current->line, lex_state.current->column);
	}

	return 1;
}

char expect_lit_or_iden(char stack) {
	char error;
	if (!expect_lit()) {
		if (stack) {
			// TODO: size (?)
			struct stack_item *item = new_stack_item(lex_state.current->tok->type - 40, 0);
			push(lex_state.stack, item);
		}

		return 0;
	}

	error = expect_iden(1, 1);
	
	if (!error) {
		if (stack) {
			// lookup identifier
			// identifier name?

			// struct stack_item *item = new_stack_item(lex_state.current->tok->type - 40, 0);
			// push(lex_state.stack, item);
		}

		return 0;
	}
	if (error == 1)
		error_m("Expected literal or identifier token", lex_state.current->line, lex_state.current->column);
	if (error == 2) {	
		char *m = malloc(128);
		sprintf(m, "Variable with name \"%s\" has not been already declared", lex_state.current->tok->content);
		error_m(m, lex_state.current->line, lex_state.current->column);
	}

	return 1;
}

char expect_lit() {
	unsigned char i;

	for (i = 0; i < L_OPERATORS; i++)
		if (!expect(l_operators[i], 0))
			break;

	for (i = 0; i < LITERALS; i++) {
		if (!expect(literals[i], 0))
			return 0;
	}

	return 1;
}

char expect_iden(char left, char right) {
	unsigned char i;
	
	if (left)
		for (i = 0; i < L_OPERATORS; i++)
			if (!expect(l_operators[i], 0))
				break;

	if (!expect(T_IDEN, 0)) {
		if (lookup_item(lex_state.variables, lex_state.current->tok->content) == NULL)
			return 2;

		if (right)
			for (i = 0; i < R_OPERATORS; i++)
				if (!expect(r_operators[i], 0))
					break;
		return 0;
	}

	return 1;
}

char expect_operator(char required) {
	if (!expect(O_SUM, 0))
		return 0;
	if (!expect(O_SUB, 0))
		return 0;
	if (!expect(O_MUL, 0))
		return 0;
	if (!expect(O_DIV, 0))
		return 0;
	if (!expect(O_MOD, 0))
		return 0;

	if (required)
		error_m("Expected operator token", lex_state.current->line, lex_state.current->column);

	return 1;
}

char expect_operation() {
	if (!expect(S_LPAR, 0)) {
		printf(">> found left parenthesis\n");
		expect_operation();
		printf(">> after nested operation\n");
		return (expect(S_RPAR, 1));
	}
	else {
		if (expect_lit_or_iden(1))
			return 1;

		printf("operand:\t%s\n", lex_state.current->tok->content);

		if (!expect_operator(0)) {
			printf("operator:\t%s\n", lex_state.current->tok->content);
			char operator = lex_state.current->val_c;

			if (expect_operation())
				return 1;

			stack_operation(stack, operator);
		}
	}

	return 0;
}

char expect_decl(enum token_type type) {
	do {
		if (expect(T_IDEN, 1))
			return 1;

		if (lookup_item(lex_state.variables, lex_state.current->tok->content) != NULL) {
			char *m = malloc(128);
			sprintf(m, "Variable with name \"%s\" has already been already declared", lex_state.current->tok->content);
			error_m(m, lex_state.current->line, lex_state.current->column);

			return 1;
		}

		if (!expect(S_LSBR, 0)) {
			if (expect_operation())
				return 1;

			if (expect(S_RSBR, 1))
				return 1;

			printf(">> pop top value from stack\n");
		}

		// swtich (lex_state.current->tok->type) {
		// 	case
		// }
		// fprntf(lex_state.output, "");

		if (!expect(S_ASGN, 0)) 
			if (expect_operation())
				return 1;
	
		// TODO: change size to reflect value in square brackets
		struct ht_item *var = new_ht_item(lex_state.current->tok->content, 0, type - 40);
		hash_item(lex_state.variables, var);
	} while (!expect(S_CMA, 0));

	if (expect_eoi())
		return 1;

	printf("Successfully parsed DECLARATION\n");
	return 0;
}

char expect_eoi() {
	if (expect(S_SCLN, 1))
		return 1;

	return 0;
}