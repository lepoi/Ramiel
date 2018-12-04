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
		case T_IDEN:
			error = expect_asgn(lookup_item(lex_state.variables, lex_state.current->tok->content));
			break;
		case K_IF:
			expect_if();
			break;
		case K_WHLE:
			expect_while();
			break;
		case K_FOR:
			expect_for();
			break;
		case K_PRNT:
			expect_print();
			break;
		case K_READ:
			expect_read();
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
		error_m(m, lex_state.current->line, lex_state.current->column);
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
		sprintf(m, "Variable with name \"%s\" has not been already declared", lex_state.current->tok->content);
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
		error_m("Expected operator token", lex_state.current->line, lex_state.current->column);

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
		char *operator = &operator_code[ope * 3];

		if (expect_operation())
			return 1;

		fprintf(lex_state.output, "%.3s\n", operator);
		return stack_operation(lex_state.stack, operator);
	}

	return 0;
}

char expect_decl(enum token_type type) {
	char *id;
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

			struct stack_item *item = pop(lex_state.stack);
			if (item->type > type - 40)
				warning_m("Assigned higher precision value to variable with lower precision, may cause unexpected behavior", lex_state.current->line, lex_state.current->column);
		}


		// TODO: change size to reflect value in square brackets
		struct ht_item *var = new_ht_item(id, 0, type - 40);
		hash_item(lex_state.variables, var);

		fprintf(lex_state.output, "POP%.1s %s\n", &d_types_code[type - 40], id);
	} while (!expect(S_CMA, 0));

	printf("Successfully parsed DECLARATION\n");
	return 0;
}

char expect_asgn(struct ht_item *item) {
	if (item == NULL) {
		char *m = malloc(128);
		sprintf(m, "Variable with name \"%s\" has not been already declared", lex_state.current->tok->content);
		error_m(m, lex_state.current->line, lex_state.current->column);

		return 1;
	}

	if (!expect(T_IDEN, 1))
		if (!expect(S_ASGN, 1))
			if (!expect_operation())
				return 0;

	return 1;
}

char expect_if() {
	if (!expect(S_LPAR, 1)) {
		if (!expect_data_type(0)) {
			// switch (lex_state.current->tok->type) {
			// 	case K_CHR:
			// 		break;
			// 	case K_INT:
			// 		break;
			// 	case K_FLT:
			// 		break;
			// 	case K_STR:
			// 		break;
			// }

			return 0;
		}
		else {

		}
	}

	return 1;
}

char expect_while() {
	return 0;
}

char expect_for() {
	return 0;
}

char expect_print() {
	return 0;
}

char expect_read() {
	return 0;
}

char expect_eoi() {
	if (expect(S_SCLN, 1))
		return 1;

	return 0;
}

// char expect_asgn(enum token_type type){
// 	char *id;
// 	id = strdup(lex_state.current->tok->content);
// 	if (!expect(S_ASGN, 1))
// 		if(!expect_operation()){
// 			struct stack_item *item = pop(lex_state.stack);
// 			if (item->type > type - 40)
// 				warning_m("Assigned higher precision value to variable with lower precision, may cause unexpected behavior", lex_state.current->line, lex_state.current->column);
// 		}
// 			if (!expect_eoi())
// 				fprintf(lex_state.output,"POP%c %s\n",lookup_item(lex_state.variables,id),id);//pop con el tipo de la variabe y nombre de la variable
// 				return 0;
// 	return 1;
// }