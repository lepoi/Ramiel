#include "parser.h"

char analyze() {
	lex_state.current = lex_state.list;

	while (lex_state.current != NULL && lex_state.current->next != NULL) {
		if (!consume_token())
			lex_state.current = lex_state.current->next;
	}

	return 0;
}

void rewind_token() {
	struct token_info *new = new_token_info(lex_state.current->tok);
	new->next = lex_state.current->next;
	new->line = lex_state.current->line;
	new->column = lex_state.current->column;
	new->negative = lex_state.current->negative;
	lex_state.current->next = new;
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
			error = expect_decl(1, lex_state.current->tok->type + 23);
			break;
		case T_IDEN:
			if (lex_state.current->next->tok->type == S_ASGN)
				error = expect_asgn(lookup_item(lex_state.variables, lex_state.current->tok->content));
			else {
				rewind_token();
				error = expect_operation();
			}
			break;
		case K_IF:
			error = expect_if(lex_state.current->line, lex_state.current->column);
			break;
		case K_WHLE:
			error = expect_while(lex_state.current->line, lex_state.current->column);
			break;
		case K_FOR:
			error = expect_for(lex_state.current->line, lex_state.current->column);
			break;
		case K_PRNT:
			error = expect_print();
			break;
		case K_READ:
			error = expect_read();
			break;
		case S_LPAR:
		case O_INC:
		case O_DEC:
			rewind_token();
			error = expect_operation();
			break;
		default:
			m = malloc(64);
			sprintf(m, "Unexpected \"%s\" - token: %.6s\n", lex_state.current->tok->content, &token_names[lex_state.current->tok->type * 8]);
			warning_m(m, lex_state.current->line, lex_state.current->column);
			error = 1;
			break;
	}

	if (error)
		skip_tokens();
	else
		return expect_eoi(1);

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
		error_m(m, lex_state.current->next->line, lex_state.current->next->column);
	}

	return 1;
}

char expect_data_type(char required) {
	int i;

	for (i = 0; i < D_TYPES; i++)
		if (!expect(d_types[i], 0))
			return 0;

	if (required)
		error_m("Expected data type keyword", lex_state.current->next->line, lex_state.current->next->column);	

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
			struct ht_item *item = lookup_item(lex_state.variables, iden);
			struct stack_item *st_item = new_stack_item(item->type, 0);
			push(lex_state.stack, st_item);
		}

		return 0;
	}
	if (error == 1)
		error_m("Expected literal or identifier token", lex_state.current->line, lex_state.current->column);
	if (error == 2) {	
		char *m = malloc(128);
		sprintf(m, "Variable with name \"%s\" has not been declared", lex_state.current->tok->content);
		error_m(m, lex_state.current->line, lex_state.current->column);
	}

	return 1;
}

char expect_lit() {
	char c [4] = { 'C', 'I', 'D', 'S' };

	for (int i = 0; i < LITERALS; i++) {
		if (!expect(literals[i], 0)) {
			fprintf(lex_state.output, "PUSHK%c %s\n", c[i], lex_state.current->tok->content);

			return 0;
		}
	}

	return 1;
}

char expect_iden(char left, char right) {
	int i;
	int found_left = -1, found_right = -1;

	if (left)
		for (i = 0; i < L_OPERATORS; i++) {	
			if (!expect(l_operators[i], 0)) {
				found_left = i;
				break;
			}
		}

	if (!expect(T_IDEN, 0)) {
		struct ht_item *var = lookup_item(lex_state.variables, lex_state.current->tok->content);
		if (!var)
			return 2;

		iden = strdup(lex_state.current->tok->content);
		char pop_t;

		switch (var->type) {
			case DATA_CHR:
				pop_t = 'C';
				break;
			case DATA_INT:
				pop_t = 'I';
				break;
			case DATA_FLT:
				pop_t = 'D';
				break;
			case DATA_STR:
				pop_t = 'S';
		}

		if (found_left != -1)
			fprintf(lex_state.output, l_operator_code[found_left], iden, pop_t, iden);

		fprintf(lex_state.output, "PUSH %s\n", iden);

		if (right)
			for (i = 0; i < R_OPERATORS; i++)
				if (!expect(r_operators[i], 0)) {
					found_right = i;
					break;
				}

		if (found_right != -1)
			fprintf(lex_state.output, r_operator_code[found_right], iden, pop_t, iden);

		return 0;
	}

	return 1;
}

char expect_operator(char required) {
	for (int i = 0; i < OPERATORS; i++)
		if (!expect(operators[i], 0)) {
			ope = i;
			return 0;
		}

	if (required)
		error_m("Expected operator token", lex_state.current->next->line, lex_state.current->next->column);

	return 1;
}

char expect_operation() {
	if (!expect(S_LPAR, 0)) {
		if (expect_operation())
			return 1;
		if (expect(S_RPAR, 1))
			return 1;
	}
	else if (expect_lit_or_iden(1))
		return 1;

	if (!expect_operator(0)) {
		char *operator = malloc(3);
		sprintf(operator, "%.3s", &operator_code[ope * 3]);

		if (expect_operation())
			return 1;

		fprintf(lex_state.output, "%.3s\n", operator);
		return stack_operation(lex_state.stack, operator);
	}

	return 0;
}

char expect_decl(char multiple, enum token_type type) {
	char *id;
	char asgn;
	do {
		if (expect(T_IDEN, 1))
			return 1;

		if (lookup_item(lex_state.variables, lex_state.current->tok->content) != NULL) {
			char *m = malloc(128);
			sprintf(m, "Variable with name \"%s\" has already been already declared", lex_state.current->tok->content);
			error_m(m, lex_state.current->line, lex_state.current->column);

			return 1;
		}
		id = strdup(lex_state.current->tok->content);
		fprintf(lex_state.output, "DCL%.1s %s\n", &d_types_code[type - 40], id);

		if (!expect(S_LSBR, 0)) {
			if (expect_operation())
				return 1;

			if (expect(S_RSBR, 1))
				return 1;

			// TODO: pop top of stack as array length
		}

		if (!expect(S_ASGN, 0)) {
			if (expect_operation())
				return 1;
			else
				asgn = 1;

			struct stack_item *item = pop(lex_state.stack);
			if (item->type > type - 40) {
				char *m = malloc(64);
				sprintf(m, "Assigned %s value to %s variable, may cause unexpected behavior", d_types_mneumonic[item->type], d_types_mneumonic[type - 40]);
				warning_m(m, lex_state.current->line, lex_state.current->column);
			}
		}


		// TODO: change size to reflect value in square brackets
		struct ht_item *var = new_ht_item(id, 0, type - 40);
		hash_item(lex_state.variables, var);

		if (asgn)
			fprintf(lex_state.output, "POP%.1s %s\n", &d_types_code[type - 40], id);

		if (!multiple)
			break;
	} while (!expect(S_CMA, 0));

	printf("Successfully parsed DECLARATION\n");
	return 0;
}

char expect_asgn(struct ht_item *item) {
	char *id = lex_state.current->tok->content;
	if (item == NULL) {
		char *m = malloc(128);
		sprintf(m, "Variable with name \"%s\" has not been declared", id);
		error_m(m, lex_state.current->line, lex_state.current->column);

		return 1;
	}
	if (expect(S_ASGN, 1))
		return 1;

	if (!expect_operation()) {
		struct stack_item *s_item = pop(lex_state.stack);

		if (s_item->type > item->type) {
			char *m = malloc(64);
			sprintf(m, "Assigned %s value to %s variable, may cause unexpected behavior", d_types_mneumonic[s_item->type], d_types_mneumonic[item->type]);
			warning_m(m, lex_state.current->line, lex_state.current->column);
		}

		fprintf(lex_state.output, "POP%.1s %s\n", &d_types_code[s_item->type], id);
		printf("Successfully parsed ASSIGN\n");
		return 0;
	}

	return 1;
}

char expect_if(unsigned short line, unsigned short column) {
	return 0;
}

char expect_while(unsigned short line, unsigned short column) {
	return 0;
}

char expect_for(unsigned short line, unsigned short column) {
	if (!expect(S_LPAR, 1)) {
		do {
			if (!expect_data_type(0)) {
				if (expect_decl(0, lex_state.current->tok->type - 17))
					return 1;
			}
			else if (!expect(T_IDEN, 0)) {
				if (expect_asgn(lookup_item(lex_state.variables, lex_state.current->tok->content)))
					return 1;
			}
			else
				break;
		} while (expect(S_CMA, 0));

		if (expect(S_SCLN, 1))
			return 1;

		fprintf(lex_state.output, "\tfor_l%dc%d_condition:\n", line, column);

		if (expect_operation())
			return 1;

		fprintf(lex_state.output, "JMPC for_l%dc%d_exit\n", line, column);
		fprintf(lex_state.output, "JMP for_l%dc%d_body\n", line, column);

		if (expect(S_SCLN, 1))
			return 1;

		fprintf(lex_state.output, "\tfor_l%dc%d_callback:\n", line, column);
		do {
			for (int i = 0; i < L_OPERATORS; i++) {
				if (!expect(l_operators[i], 0)) {
					rewind_token();
					if (expect_operation())
						return 1;
					
					continue;
				}
			}
		
			if (expect(T_IDEN, 1))
				return 1;

			// TODO: add brackets for array elements

			if (lex_state.current->next->tok->type == S_ASGN) {
				if (expect_asgn(lookup_item(lex_state.variables, lex_state.current->tok->content)))
					return 1;
			}
			else {
				rewind_token();
				expect_operation();
			}
		} while (!expect(S_CMA, 0));

		if (expect(S_RPAR, 1))
			return 1;

		fprintf(lex_state.output, "JMP for_l%dc%d_codition\n", line, column);

		if (expect(S_LCBR, 1))
			return 1;

		fprintf(lex_state.output, "\tfor_l%dc%d_body:\n", line, column);

		if (expect_body())
			return 1;

		fprintf(lex_state.output, "JMP for_l%dc%d_callback\n", line, column);

		if (expect(S_RCBR, 1))
			return 1;

		fprintf(lex_state.output, "\tfor_l%dc%d_exit:\n", line, column);
		printf("Successfully parsed FOR_CYCLE\n");
		return 0;
	}
	return 1;
}

char expect_print() {
	return 0;
}

char expect_read() {
	return 0;
}

char expect_body() {
	char error;

	if (lex_state.current->next)
		lex_state.current = lex_state.current->next;

	while (lex_state.current->tok->type != S_RCBR) {
		error = consume_body_token();
		if (!error)
			lex_state.current = lex_state.current->next;
	}
	rewind_token();

	return 0;
}

void skip_body_tokens() {
	do {
		if (lex_state.current->tok->type == T_EOF) {
			error_m("Unexpected end of file, colsing '}' expected", lex_state.current->line, lex_state.current->column);
			break;
		}
		lex_state.current = lex_state.current->next;
	} while (lex_state.current->tok->type != S_SCLN && lex_state.current->tok->type != S_LCBR);

	lex_state.current = lex_state.current->next;
}

char consume_body_token() {
	char *m;
	char error;

	switch (lex_state.current->tok->type) {
		case S_RCBR:
			return 0;
		case K_INT:
		case K_FLT:
		case K_CHR:
		case K_STR:
			error = expect_decl(1, lex_state.current->tok->type + 23);
			break;
		case T_IDEN:
			if (lex_state.current->next->tok->type == S_ASGN)
				error = expect_asgn(lookup_item(lex_state.variables, lex_state.current->tok->content));
			else {
				rewind_token();
				error = expect_operation();
			}
			break;
		case K_IF:
			error = expect_if(lex_state.current->line, lex_state.current->column);
			break;
		case K_WHLE:
			error = expect_while(lex_state.current->line, lex_state.current->column);
			break;
		case K_FOR:
			error = expect_for(lex_state.current->line, lex_state.current->column);
			break;
		case K_PRNT:
			error = expect_print();
			break;
		case K_READ:
			error = expect_read();
			break;
		case S_LPAR:
		case O_INC:
		case O_DEC:
			rewind_token();
			error = expect_operation();
			break;
		default:
			m = malloc(64);
			sprintf(m, "Unexpected token %.8s\n", &token_names[lex_state.current->tok->type * 8]);
			warning_m(m, lex_state.current->line, lex_state.current->column);
			error = 1;
			break;
	}

	if (error)
		skip_body_tokens();
	else
		return expect_eoi(1);

	return error;
}

char expect_eoi() {
	if (expect(S_SCLN, 1))
		return 1;

	return 0;
}