char verify_syntax() {
	lex_state.current = lex_state.list;

	while (lex_state.current != NULL && lex_state.current->next != NULL) {
		if (!consume_token())
			lex_state.current = lex_state.current->next;
	}

	return 0;
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
		sprintf(m, "Expected %.8s, found %.8s", &token_names[type * 8], &token_names[lex_state.current->next->tok->type * 8]);
		error_m(m, lex_state.current->line, lex_state.current->column);
	}

	return 1;
}

void skip_tokens() {
	do {
		if (lex_state.current->tok->type == T_EOF)
			break;
		lex_state.current = lex_state.current->next;
	} while (lex_state.current->tok->type != S_SCLN && lex_state.current->tok->type != S_LCBR);
}

char consume_token() {
	char error = 0;
	switch (lex_state.current->tok->type) {
		case K_INT:
		case K_FLT:
		case K_CHR:
		case K_STR:
			error = expect_decl();
			break;
		default:
			printf("[ switch default ]\n");
			error = 1;
			break;
	}

	if (error)
		skip_tokens();

	return error;
}

char expect_decl() {
	do {
		if (expect(T_IDEN, 1))
			return 1;

		if (!expect(S_ASGN, 0)) 
			if (expect_lit_or_iden())
				return 1;
	
	} while (!expect(S_CMA, 0));

	if (expect(S_SCLN, 1))
		return 1;

	printf("Int declaration\n");
	return 0;
}

char expect_lit_or_iden() {
	if (!expect_lit())
		return 0;

	if (!expect_iden(1, 1))
		return 0;

	error_m("Expected T_IDEN or literal token", lex_state.current->line, lex_state.current->column);
	return 1;
}

char expect_lit() {
	unsigned char i;

	for (i = 0; i < L_OPERATORS; i++)
		if (!expect(l_operators[i], 0))
			break;

	for (i = 0; i < D_TYPES; i++) {
		if (!expect(d_types[D_TYPES], 0))
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
		if (right)
			for (i = 0; i < R_OPERATORS; i++)
				if (!expect(r_operators[i], 0))
					break;
		return 0;
	}

	return 1;
}