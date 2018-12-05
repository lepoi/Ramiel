char *token_names = "O_SUM   O_INC   O_SUB   O_DEC   O_MUL   O_DIV   O_MOD   O_NOT   O_NEQ   O_LES   O_LEEQ  O_GRE   O_GREQ  S_ASGN  O_EQ    O_AND   O_OR    K_CHR   K_INT   K_FLT   K_STR   K_IF    K_EIF   K_ELSE  K_WHLE  K_FOR   K_PRNT  K_READ  K_BRK   K_CONT  S_LPAR  S_RPAR  S_LCBR  S_RCBR  S_LSBR  S_RSBR  S_SCLN  S_PNT   S_CMA   T_IDEN  T_CHR   T_INT   T_FLT   T_STR   T_EOF";

enum token_type {
	O_SUM,	O_INC,	O_SUB,	O_DEC,	O_MUL,	O_DIV,	O_MOD,	O_NOT,	O_NEQ,
	O_LES,	O_LEEQ,	O_GRE,	O_GREQ,	S_ASGN,	O_EQ,	O_AND,	O_OR,	K_CHR,
	K_INT,	K_FLT,	K_STR,	K_IF,	K_EIF,	K_ELSE,	K_WHLE,	K_FOR,	K_PRNT,
	K_READ,	K_BRK,	K_CONT,	S_LPAR,	S_RPAR,	S_LCBR,	S_RCBR,	S_LSBR,	S_RSBR,
	S_SCLN,	S_PNT,	S_CMA,	T_IDEN,	T_CHR,	T_INT,	T_FLT,	T_STR,	T_EOF
};

#define D_TYPES 4
#define LITERALS 4
#define OPERATORS 13
#define L_OPERATORS 2
#define R_OPERATORS 2

enum token_type d_types [D_TYPES] = { K_CHR, K_INT, K_FLT, K_STR };
enum token_type literals [LITERALS] = { T_CHR, T_INT, T_FLT, T_STR };
enum token_type operators [OPERATORS] = { O_SUM, O_SUB, O_MUL, O_DIV, O_MOD, O_EQ, O_NEQ, O_GRE, O_GREQ, O_LES, O_LEEQ, O_AND, O_OR };
enum token_type l_operators [L_OPERATORS] = { O_INC, O_DEC };
enum token_type r_operators [R_OPERATORS] = { O_INC, O_DEC };

char *d_types_code = "CIDS";
char *d_types_mneumonic [D_TYPES] = {
	"char",
	"integer",
	"float",
	"string"
};
char *operator_code = "ADDSUBMULDIVMODCEQCNECGTCGECLTCLE";
char *l_operator_code [L_OPERATORS] = {
	"PUSH %s\nPUSHKI 1\nADD\nPOP%c %s\n",
	"PUSH %s\nPUSHKI 1\nSUB\nPOP%c %s\n"
};
char *r_operator_code [R_OPERATORS] = {
	"PUSH %s\nPUSHKI 1\nADD\nPOP%c %s\n",
	"PUSH %s\nPUSHKI 1\nSUB\nPOP%c %s\n"
};

struct token {
	enum token_type type;
	char *content;
};

struct token_info {
	struct token *tok;
	struct token_info *next;
	ushort line, column;
	union {
		int val_i;
		double val_d;
		char val_c;
		char *val_s;
	};
	unsigned char negative;
};

struct token_info *new_token_info(struct token *n_token) {
	struct token_info *t_info = malloc(sizeof(struct token_info));
	t_info->tok = n_token;
	t_info->negative = 0;

	return t_info;
}