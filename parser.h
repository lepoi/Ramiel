#include "hashtable.c"
#include "stack.c"

char analyze();
void skip_tokens();
char consume_token();
char expect(enum token_type type, char required);
char expect_lit_or_iden();
char expect_lit();
char expect_iden(char left, char right);
char expect_operator(char required);
char expect_operation();
char expect_decl();
char expect_asgn();
char expect_while();
char expect_for();
char expect_print();
char expect_read();
char expect_eoi();