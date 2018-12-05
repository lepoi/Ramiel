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

struct operator_stack *new_ope_stack() {
	struct operator_stack *stack = malloc(sizeof(struct operator_stack));
	stack->top = NULL;
	stack->count = 0;

	return stack;
}

struct ope_stack_item *new_ope_stack_item(char *ope) {
	struct ope_stack_item *item = malloc(sizeof(struct ope_stack_item));
	item->ope = strdup(ope);

	if (ope[0] == '(')
		item->prio = 0;
	else if (ope[0] == ')')
		item->prio = 0;
	// Comparisons
	else if (ope[0] == 'C')
		item->prio = 4;
	// Multiplucation, Module
	else if (ope[0] == 'M')
		item->prio = 3;
	else if (strcmp(ope, "DIV") == 0)
		item->prio = 3;
	else if (strcmp(ope, "SUM") == 0)
		item->prio = 2;
	else if (strcmp(ope, "SUB") == 0)
		item->prio = 2;

	item->next = NULL;

	return item;
}

void push_ope(struct operator_stack *stack, struct ope_stack_item *item) {
	item->next = stack->top;
	stack->top = item;
	stack->count++;
}

struct ope_stack_item *pop_ope(struct operator_stack *stack) 	{
	if (stack->count == 0)
		return NULL;

	struct ope_stack_item *item = stack->top;
	stack->top = stack->top->next;
	item->next = NULL;
	stack->count--;

	return item;
}