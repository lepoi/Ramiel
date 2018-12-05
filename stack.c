#include "stack.h"

struct stack *new_stack() {
	struct stack *stack = malloc(sizeof(struct stack));
	stack->top = NULL;
	stack->count = 0;

	return stack;
}

struct stack_item *new_stack_item(enum data_t type, char array) {
	struct stack_item *item = malloc(sizeof(struct stack_item));
	item->type = type;
	item->array = array;
	item->next = NULL;

	return item;
}

void push(struct stack *stack, struct stack_item *item) {
	item->next = stack->top;
	stack->top = item;
	stack->count++;
}

struct stack_item *pop(struct stack *stack) {
	if (stack->count == 0)
		return NULL;

	struct stack_item *item = stack->top;
	stack->top = stack->top->next;
	item->next = NULL;
	stack->count--;

	return item;
}

char stack_operation(struct stack *stack, char *operator) {

	struct stack_item *item1 = pop(stack);
	struct stack_item *item2 = pop(stack);

	if (!(item1 && item2))
		return 1;

	// if (item1->array || item2->array) {
	// 	error_m("Cannot perform operations with arrays", lex_state.line, lex_state.column);
	// 	return 1;
	// }

	if ((item1->type == DATA_STR || item2->type == DATA_STR) && strcmp(operator, "+") != 0) {
		error_m("Only sum operations are permitted with string type", lex_state.line, lex_state.column);
		return 1;
	}

	enum data_t type;

	if (operator[0] == 'C')
		type = DATA_INT;
	else
		type = max(item1->type, item2->type);

	struct stack_item *item = new_stack_item(type, max(item1->array, item2->array));
	push(stack, item);

	return 0;
}