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