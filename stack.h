struct stack_item {
	enum data_t type;
	union {
		char val_c;
		int val_i;
		double val_d;
		char *val_s;
	};
	size_t size;
	struct stack_item *next;
};

struct stack {
	struct stack_item *top;
	size_t count;
};

struct stack *new_stack();
struct stack_item *new_stack_item(enum data_t type, size_t size);

void push(struct stack *stack, struct stack_item *item);
struct stack_item *pop(struct stack *stack);