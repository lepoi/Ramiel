#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "utils.c"
#include "tokens.c"
#include "state.c"
#include "console.c"
#include "syntax.c"

struct token_info *info;

char make_tokens();
char next_char();
void return_char(char ret);
void skip_line();
char parse(struct token_info *n_info, char current);
char parse_char(struct token_info *n_info);
char parse_string(struct token_info *n_info);
char parse_word_or_number(struct token_info *n_info, char first);
char parse_word(struct token_info *n_info, char first);
char parse_number(struct token_info *n_info, char first);