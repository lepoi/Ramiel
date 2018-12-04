#include "hashtable.c"
#include "stack.c"

char *iden;
int ope;

char analyze();
void skip_tokens();
char consume_token();
char expect(enum token_type type, char required);
char expect_data_type(char required);
char expect_lit_or_iden(char stack);
char expect_lit();
char expect_iden(char left, char right);
char expect_operator(char required);
char expect_operation();
char expect_decl();
char expect_asgn(struct ht_item *item);
char expect_while();
char expect_if();
char expect_while();
char expect_for();
char expect_print();
char expect_read();
char expect_eoi();