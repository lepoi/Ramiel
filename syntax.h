char verify_syntax();
char expect(enum token_type type, char required);
void skip_tokens();
char consume_token();
char expect_decl();
char expect_lit_or_iden();
char expect_lit();
char expect_iden(char left, char right);