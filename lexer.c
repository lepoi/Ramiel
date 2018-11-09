#include "lexer.h"

int main(int argc, char *args[]) {
	if (!args[1]) {
		printf("Needs more arguments!\n");
		exit(1);
	}

	struct state lex_state = new_state();
	lex_state.input = fopen(args[1], "r");	
	if (!lex_state.input) {
		printf("Invalid file\n");
		exit(1);
	}

	lex_state.output = fopen("out.tok", "wb");
	if (!lex_state.output) {
		printf("Cannot open destination file \"out.tok\"\n");
		exit(1);
	}
	char current = next_char(&lex_state), error = 0;
	struct token_info *n_info = malloc(sizeof(struct token_info));

	fprintf(lex_state.output, "%6s %8s %8s\t%s\n\n", "line", "column", "token", "content");

	while (current != EOF) {
		n_info->line = lex_state.line;
		n_info->column = lex_state.column;
		error = parse(n_info, current, &lex_state);

		if (!error) {
			fprintf(lex_state.output, "%6d %8d %.8s\t%s\n", lex_state.line, lex_state.column, &token_names[n_info->tok->type * 8], n_info->tok->content);
		}

		current = next_char(&lex_state);
	}
	printf("End Of File reached.\n");

	fclose(lex_state.input);
	fclose(lex_state.output);

	return 0;
}

char parse(struct token_info *n_info, char current, struct state *lex_state) {
	struct token *tok = malloc(sizeof(struct token));
	n_info->tok = tok;

	switch (current) {
		// whitespace
		case ' ':
		case '\t':
		case '\n':
			tok->type = T_WHSP;
			break;

		// operators
		case '*':
			tok->type = O_MUL;
			break;
		case '/':
			tok->type = O_DIV;
			break;
		case '%':
			tok->type = O_MOD;
			break;
		case '+':
			tok->type = O_SUM;
			break;
		case '-':
			tok->type = O_SUB;
			break;
		case '!':
			tok->type = O_NEG;
			break;
		case '<':
			tok->type = O_LES;
			break;
		case '>':
			tok->type = O_GRE;
			break;
		case '&':
			tok->type = O_AND;
			break;
		case '|':
			tok->type = O_OR;
			break;
		case '=':
			tok->type = O_EQ;
			break;

		// symbols
		case '(':
			tok->type = S_RPAR;
			break;
		case ')':
			tok->type = S_RPAR;
			break;
		case '{':
			tok->type = S_LCBR;
			break;
		case '}':
			tok->type = S_RCBR;
			break;
		case '[':
			tok->type = S_LSBR;
			break;
		case ']':
			tok->type = S_RSBR;
			break;
		case '.':
			tok->type = S_PNT;
			break;
		case ',':
			tok->type = S_CMA;
			break;
		case ';':
			tok->type = S_SCLN;
			break;

		// char literal
		case '\'':
			return parse_char(n_info, lex_state);
		
		// string literal
		case '"':
			return parse_string(n_info, lex_state);
		
		// keyword, identifier or number
		default:
			return parse_word_or_number(n_info, current, lex_state);	
	}
	tok->content = &current;
	n_info->val_c = current;

	return 0;
}

char next_char(struct state *lex_state) {
	char read = (char) fgetc(lex_state->input);
	lex_state->column++;
	
	if (read == EOF)
		return EOF;

	if (read == '\n') {
		lex_state->line++;
		lex_state->column = 0;
	}

	return read;
}

void return_char(char ret, struct state *lex_state) {
	ungetc(ret, lex_state->input);
	lex_state->column--;
}

void skip_line(struct state *lex_state) {
	char r;
	while ((r = next_char(lex_state)) != '{' && r != '\n' && r != EOF);
}

struct token *consume_token(struct state *lex_state) {
	char *buffer;
	struct token *new_token;
	if (fscanf(lex_state->input, buffer) > 0) {
		// TODO: no saber que pedo xD
		printf("%s\n", buffer);
	}

	return new_token;
}

char parse_char(struct token_info *n_info, struct state *lex_state) {
	char char_1 = next_char(lex_state), char_2;

	if (char_2 = next_char(lex_state) != '\'') {
		error_m("Error in character literal, closing \' expected", lex_state, n_info->line, n_info->column);
		return_char(char_2, lex_state);
		skip_line(lex_state);
	}
	else {
		struct token *tok = malloc(sizeof(struct token));
		tok->type = T_CHR;
		tok->content = &char_1;
		n_info->tok = tok;
		n_info->val_c = char_1;

		return 0;
	}

	return 1;
}

char parse_string(struct token_info *n_info, struct state *lex_state) {
	char read = next_char(lex_state);
	char *string = "";

	while (read != '\"') {
		string = append(string, read);
		read = next_char(lex_state);

		if (read == EOF) {
			error_m("Error in string literal, closing \" expected", lex_state, n_info->line, n_info->column);
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

char parse_word_or_number(struct token_info *n_info, char first, struct state *lex_state) {
	if (isdigit(first))
		return parse_number(n_info, first, lex_state);
	else
		return parse_word(n_info, first, lex_state);
}

char parse_word(struct token_info *n_info, char first, struct state *lex_state) {
	char read = first, keyword = 0;
	char *word = "";
	struct token *tok = malloc(sizeof(struct token));
	n_info->tok = tok;

	while (isalpha(read) || isdigit(read)) {
		word = append(word, read);
		read = next_char(lex_state);
	}

	tok->type = T_IDEN;

	if (strcmp(word, "int") == 0)
		tok->type = T_INT;
	else if (strcmp(word, "float") == 0)
		tok->type = T_FLT;
	else if (strcmp(word, "char") == 0)
		tok->type = T_CHR;
	else if (strcmp(word, "string") == 0)
		tok->type = T_STR;
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

	return_char(read, lex_state);
	return 0;
}

char parse_number(struct token_info *n_info, char first, struct state *lex_state) {
	char read = first;
	char *number = "";
	struct token *tok = malloc(sizeof(struct token));
	n_info->tok = tok;

	while (isdigit(read)) {
		number = append(number, read);
		read = next_char(lex_state);
	}

	if (read == '.') {
		number = append(number, '.');
		read = next_char(lex_state);

		while (isdigit(read)) {
			number = append(number, read);
			read = next_char(lex_state);
		}

		tok->type = T_FLT;
		tok->content = number;
		n_info->val_d = atof(number);
	
		return_char(read, lex_state);
		return 0;
	}

	tok->type = T_INT;
	tok->content = number;
	n_info->val_i = atoi(number);

	return_char(read, lex_state);
	return 0;
}