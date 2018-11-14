char *token_names = "O_SUM   O_INC   O_SUB   O_DEC   O_MUL   O_DIV   O_MOD      O_NOT   O_NEQ   O_LES  O_LEEQ   O_GRE  O_GREQ  S_ASGN    O_EQ   O_AND    O_OR   K_INT   K_FLT   K_CHR   K_STR    K_IF   K_EIF  K_ELSE  K_WHLE   K_FOR  K_PRNT  K_READ  S_LPAR  S_RPAR  S_LCBR  S_RCBR  S_LSBR  S_RSBR  S_SCLN   S_PNT   S_CMA  T_IDEN   T_INT   T_FLT   T_CHR   T_STR  T_WHSP   T_EOF";

enum token_type {
	O_SUM,	O_INC,	O_SUB,	O_DEC,	O_MUL,	O_DIV,	O_MOD,	O_NOT,	O_NEQ,
	O_LES,	O_LEEQ,	O_GRE,	O_GREQ,	S_ASGN,	O_EQ,	O_AND,	O_OR,	K_INT,
	K_FLT,	K_CHR,	K_STR,	K_IF,	K_EIF,	K_ELSE,	K_WHLE,	K_FOR,	K_PRNT,
	K_READ,	S_LPAR,	S_RPAR,	S_LCBR,	S_RCBR,	S_LSBR,	S_RSBR,	S_SCLN,	S_PNT,
	S_CMA,	T_IDEN,	T_INT,	T_FLT,	T_CHR,	T_STR,	T_WHSP,	T_EOF
};

struct token {
	enum token_type type;
	char *content;
};

struct token_info {
	struct token *tok;
	struct token_info *next;
	unsigned short line, column;
	union {
		int val_i;
		double val_d;
		char val_c;
		char *val_s;
	};
};

struct token_info *new_token_info(struct token *n_token) {
	struct token_info *t_info = malloc(sizeof(struct token_info));
	t_info->tok = n_token;

	return t_info;
}