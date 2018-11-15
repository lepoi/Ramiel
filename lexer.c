#include "lexer.h"

char make_tokens() {
	char current = next_char(), error = 0;
	while (current != EOF) {
		struct token_info *n_info = malloc(sizeof(struct token_info));
		struct token *n_tok = malloc(sizeof(struct token));
		n_info->tok = n_tok;
		n_info->line = lex_state.line;
		n_info->column = lex_state.column;
		error = parse(n_info, current);

		if (!error)
			append_token(n_info);
		
		current = next_char(&lex_state);
	}

	struct token *eof_tok = malloc(sizeof(struct token));
	eof_tok->type = T_EOF;
	eof_tok->content = "";
	struct token_info *eof_info = new_token_info(eof_tok);
	eof_info->next = NULL;
	eof_info->line = lex_state.line;
	eof_info->column = lex_state.column;
	append_token(eof_info);

	// debugging
	struct token_info *pointer = lex_state.list;
	while (pointer != NULL) {
		printf("%6d %8d %.8s\t%s\n", pointer->line, pointer->column, &token_names[pointer->tok->type * 8], pointer->tok->content);
		pointer = pointer->next;
	}

	fclose(lex_state.input);
	return error;
}

char next_char() {
	char read = (char) fgetc(lex_state.input);
	lex_state.column++;
	
	if (read == EOF)
		return EOF;

	if (read == '\n') {
		lex_state.line++;
		lex_state.column = 0;
	}

	return read;
}

void return_char(char ret) {
	ungetc(ret, lex_state.input);
	lex_state.column--;
}

void skip_line() {
	char r;
	while ((r = next_char()) != '{' && r != '\n' && r != EOF);
}

char parse(struct token_info *n_info, char current) {
	struct token *tok = n_info->tok;
	char unary = 1;

	switch (current) {
		// operators
		case '*':
			tok->type = O_MUL;
			tok->content = "*";
			break;
		case '/':
			tok->type = O_DIV;
			tok->content = "/";
			break;
		case '%':
			tok->type = O_MOD;
			tok->content = "%";
			break;
		case '+':
			tok->type = O_SUM;
			tok->content = "+";
			unary = 0;
			break;
		case '-':
			tok->type = O_SUB;
			tok->content = "-";
			unary = 0;
			break;
		case '!':
			tok->type = O_NOT;
			tok->content = "!";
			unary = 0;
			break;
		case '<':
			tok->type = O_LES;
			tok->content = "<";
			unary = 0;
			break;
		case '>':
			tok->type = O_GRE;
			tok->content = ">";
			unary = 0;
			break;
		case '&':
			tok->type = O_AND;

			tok->content = "&";
			break;
		case '|':
			tok->type = O_OR;
			tok->content = "|";
			break;
		case '=':
			tok->type = S_ASGN;
			tok->content = "=";
			unary = 0;
			break;

		// symbols
		case '(':
			tok->type = S_RPAR;
			tok->content = "(";
			break;
		case ')':
			tok->type = S_RPAR;
			tok->content = ")";
			break;
		case '{':
			tok->type = S_LCBR;
			tok->content = "{";
			break;
		case '}':
			tok->type = S_RCBR;
			tok->content = "}";
			break;
		case '[':
			tok->type = S_LSBR;
			tok->content = "[";
			break;
		case ']':
			tok->type = S_RSBR;
			tok->content = "]";
			break;
		case '.':
			tok->type = S_PNT;
			tok->content = ".";
			break;
		case ',':
			tok->type = S_CMA;
			tok->content = ",";
			break;
		case ';':
			tok->type = S_SCLN;
			tok->content = ";";
			break;

		// char literal
		case '\'':
			return parse_char(n_info);
		
		// string literal
		case '"':
			return parse_string(n_info);
		
		// keyword, identifier or number
		default:
			return parse_word_or_number(n_info, current);	
	}
	n_info->val_c = current;
	
	// multi-symbol operators
	if (!unary) {
		char next = next_char();
		if (current == '<' || current == '>' || current == '=' || current == '!') {
			if (next == '=') {
				tok->type++;
				tok->content = append(tok->content, '=');
				n_info->val_s = tok->content;
			}
			else
				return_char(next);
		}
		else if (current == '+' && next == '+') {
			tok->type = O_INC;
			tok->content = "++";
			n_info->val_s = tok->content;
		}
		else if (current == '-' && next == '-') {
			tok->type = O_DEC;
			tok->content = "++";
			n_info->val_s = tok->content;

		}
		else
			return_char(next);
	}

	return 0;
}

char parse_char(struct token_info *n_info) {
	char char_1 = next_char(), char_2;

	if ((char_2 = next_char()) != '\'') {
		error_m("Error in character literal, closing \' expected", n_info->line, n_info->column);
		return_char(char_2);
	}
	else {
		struct token *tok = malloc(sizeof(struct token));
		tok->type = T_CHR;
		tok->content = malloc(1);
		tok->content[0] = char_1;
		n_info->tok = tok;
		n_info->val_c = char_1;

		return 0;
	}

	return 1;
}

char parse_string(struct token_info *n_info) {
	char read = next_char();
	char *string = "";

	while (read != '\"') {
		string = append(string, read);
		read = next_char();

		if (read == EOF) {
			error_m("Error in string literal, closing \" expected", n_info->line, n_info->column);
			return 1;
		}
	}

	struct token *tok = malloc(sizeof(struct token));
	tok->type = T_STR;
	tok->content = string;
	n_info->tok = tok;
	n_info->val_s = string;

	return 0;
}

char parse_word_or_number(struct token_info *n_info, char first) {
	if (isdigit(first))
		return parse_number(n_info, first);
	else if (isalpha(first))
		return parse_word(n_info, first);
	else
		return 1;
}

char parse_word(struct token_info *n_info, char first) {
	char read = first;
	char *word = "";
	struct token *tok = malloc(sizeof(struct token));
	n_info->tok = tok;

	while (isalpha(read) || isdigit(read) || read == '_') {
		word = append(word, read);
		read = next_char();
	}

	tok->type = T_IDEN;

	if (strcmp(word, "int") == 0)
		tok->type = K_INT;
	else if (strcmp(word, "float") == 0)
		tok->type = K_FLT;
	else if (strcmp(word, "char") == 0)
		tok->type = K_CHR;
	else if (strcmp(word, "string") == 0)
		tok->type = K_STR;
	else if (strcmp(word, "if") == 0)
		tok->type = K_IF;
	else if (strcmp(word, "elif") == 0)
		tok->type = K_EIF;
	else if (strcmp(word, "else") == 0)
		tok->type = K_ELSE;
	else if (strcmp(word, "while") == 0)
		tok->type = K_WHLE;
	else if (strcmp(word, "for") == 0)
		tok->type = K_FOR;
	else if (strcmp(word, "print") == 0)
		tok->type = K_PRNT;
	else if (strcmp(word, "read") == 0)
		tok->type = K_READ;
	
	tok->content = word;
	n_info->val_s = word;

	return_char(read);
	return 0;
}

char parse_number(struct token_info *n_info, char first) {
	char read = first;
	char *number = "";
	struct token *tok = malloc(sizeof(struct token));
	n_info->tok = tok;

	while (isdigit(read)) {
		number = append(number, read);
		read = next_char();
	}

	if (read == '.') {
		number = append(number, '.');
		read = next_char();

		while (isdigit(read)) {
			number = append(number, read);
			read = next_char();
		}

		tok->type = T_FLT;
		tok->content = number;
		n_info->val_d = atof(number);
	
		return_char(read);
		return 0;
	}

	tok->type = T_INT;
	tok->content = number;
	n_info->val_i = atoi(number);

	return_char(read);
	return 0;
}

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