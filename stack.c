#include "stack.h"

struct stack *new_stack() {
	struct stack *stack = malloc(sizeof(struct stack));
	stack->top = NULL;
	stack->count = 0;

	return stack;
}

struct stack_item *new_stack_item(enum data_t type, size_t size) {
	struct stack_item *item = malloc(sizeof(struct stack_item));
	item->type = type;
	item->size = size;
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