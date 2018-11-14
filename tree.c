enum tree_type {

}

struct tree_node {
	struct token_info *token;
	enum tree_type type;
	struct tree *left;
	struct tree *right;
}