#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "utils.c"
#include "state.c"
#include "console.c"
#include "tokens.c"

int main(int argc, char *args[]);
char next_char(struct state *lex_state);
struct token *consume_token(struct state *lex_state);
char parse(struct token_info *n_info, char current, struct state *lex_state);
char parse_char(struct token_info *n_info, struct state *lex_state);
char parse_string(struct token_info *n_info, struct state *lex_state);
char parse_word_or_number(struct token_info *n_info, char first, struct state *lex_state);
char parse_word(struct token_info *n_info, char first, struct state *lex_state);
char parse_number(struct token_info *n_info, char first, struct state *lex_state);