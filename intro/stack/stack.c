// LIFO stack implementation
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

struct Stack {
	int ptr;
	unsigned capacity;
	int* array;
};

struct Stack* create(unsigned capacity) {
	struct Stack* stack = (struct Stack*)malloc(sizeof(struct Stack));

	stack->capacity = capacity;
	stack->ptr = -1;
	stack->array = (int*)malloc(capacity * sizeof(int));

	return stack;
}

int isFull(struct Stack* stack) {
	return stack->ptr == (int) stack->capacity-1;
}

int isEmpty(struct Stack* stack) {
	return stack->ptr == -1;
}

void push(struct Stack* stack, int val) {
	if (isFull(stack)) {
		return;
	}

	stack->array[++stack->ptr] = val;
}

int pop(struct Stack* stack) {
	if (isEmpty(stack)) {
		return INT_MIN;
	}

	return stack->array[stack->ptr--];
}

int top(struct Stack* stack) {
	if (isEmpty(stack)) {
		return INT_MIN;
	}

	return stack->array[stack->ptr];
}

int main(void) {
	struct Stack* stack = create(5);
	push(stack, 10);
	printf("Top is %d", top(stack));
	push(stack, 12);
	printf("\nTop is %d", top(stack));
	push(stack, 5);
	printf("\nTop is %d", top(stack));
	push(stack, 100);
	printf("\nTop is %d", top(stack));
	push(stack, 44);
	printf("\nTop is %d\n", top(stack));

	for (int i = 0; i < (int) stack->capacity; i++) {
		printf("\nPopping stack, ");
		printf("value: %d", pop(stack));
	}

	return 0;
}
