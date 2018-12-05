struct stack_item {
	enum data_t type;
	char array;
	struct stack_item *next;
};

struct stack {
	struct stack_item *top;
	size_t count;
};

struct stack *new_stack();
struct stack_item *new_stack_item(enum data_t type, char array);

void push(struct stack *stack, struct stack_item *item);
struct stack_item *pop(struct stack *stack);

char stack_operation(struct stack *stack, char *operator);

struct ope_stack_item {
	char *ope;
	char prio;
	struct ope_stack_item *next;
};

struct operator_stack {
	struct ope_stack_item *top;
	size_t count;
};

struct operator_stack *new_ope_stack();
struct ope_stack_item *new_ope_stack_item(char *ope);

void push_ope(struct operator_stack *stack, struct ope_stack_item *item);
struct ope_stack_item *pop_ope(struct operator_stack *stack);