#include "hashtable.c"
#include "stack.c"

char *iden;
int ope;

char analyze();
void rewind_token();
void skip_tokens();
char consume_token();
char expect(enum token_type type, char required);
char expect_data_type(char required);
// char expect_array_lit(enum data_t type, ushort elements, char required);
char expect_lit_or_iden(char stack);
char expect_lit();
char expect_iden(char left, char right);
char expect_operator(char required);
char expect_operation();
char expect_decl(char multiple, enum token_type type);
char expect_asgn(struct ht_item *item);
char expect_if(unsigned short line, unsigned short column);
char expect_while(unsigned short line, unsigned short column);
char expect_for(unsigned short line, unsigned short column);
char expect_print();
char expect_read();
char expect_body();
void skip_body_tokens();
char consume_body_token();
char expect_eoi();