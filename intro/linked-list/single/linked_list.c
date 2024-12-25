/**
* Current supported operations:
* Create:
* - first
* - last
* - at index
*
* Delete:
* - first
* - last
* - at index
*
* Searching
* Length
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct Node {
	int data;
	struct Node* next;
};

int len(struct Node* head) {
	int length = 0;

	struct Node* curr = head;
	while (curr != NULL) {
		length++;
		curr = curr->next;
	}

	return length;
}

bool searchList(struct Node* head, int target) {
	while (head != NULL) {
		if (head->data == target) {
			return true;
		}

		head = head->next;
	}

	return false;
}

struct Node* createNode(int data) {
	struct Node* new = (struct Node*)malloc(sizeof(struct Node));
	new->data = data;
	new->next = NULL;
	return new;
}

/// Deletion
struct Node* deleteFirst(struct Node* head) {
	if (head == NULL) {
		return NULL;
	}

	struct Node* temp = head;
	head = temp->next;

	// This is why C is so dangerous right here
	free(temp);

	return head;
}

struct Node* deleteAt(struct Node* head, int pos) {
	if (head == NULL) {
		return NULL;
	}

	struct Node* current = head;
	int currentPos = 0;
	while (current->next != NULL && ++currentPos < pos-1) {
		current = current->next;
	}

	// Remove this element, we need to relink
	struct Node* target = current->next;
	if (target == NULL) {
		return NULL;
	}

	struct Node* targetNext = target->next;
	if (targetNext == NULL) {
		// In this case, we just pop the element, clear our head pointer and free mem
		current->next = NULL;
		free(target);
	} else {
		// On this case, remove the element, set pointer of previous to correct
		current->next = targetNext;
		free(target);
	}

	return head;
}

struct Node* deleteLast(struct Node* head) {
	if (head == NULL) {
		return NULL;
	}

	struct Node* current = head;
	while (current->next != NULL && current->next->next != NULL) {
		current = current->next;
	}

	struct Node* temp = current->next;
	current->next = NULL;
	free(temp);

	return head;
}

/// Insertion
struct Node* insertBeginning(struct Node* head, int data) {
	struct Node* new = createNode(data);
	new->next = head;
	return new;
}

struct Node* insertAt(struct Node* head, int data, int position) {
	struct Node* new = createNode(data);

	if (head == NULL) {
		return new;
	}

	int index = 0;
	struct Node* current = head;
	while (index != position -1 && current->next != NULL) {
		current = current->next;
		index++;
	}

	// This is the position we wanna insert new at, get ref to the next node first
	struct Node* next = current->next;
	if (next == NULL) {
		current->next = new;
		return head;
	}

	new->next = next;
	current->next = new;
	return head;
}

struct Node* insertEnd(struct Node* head, int data) {
	struct Node* new = createNode(data);

	if (head == NULL) {
		return new;
	}

	struct Node* current = head;
	while (current->next != NULL) {
		current = current->next;
	}

	current->next = new;
	return head;
}

int main(void) {
	struct Node* head = insertEnd(NULL, 1);
	insertEnd(head, 2);
	insertEnd(head, 3);
	insertEnd(head, 4);
	insertEnd(head, 6);
	insertAt(head, 5, 3);
	head = deleteFirst(head);
	deleteLast(head);
	deleteAt(head, 1);

	// Traverse the list and printf
	struct Node* current = head;
	while (current != NULL) {
		printf("%d", current->data);
		current = current->next;
	}

	printf("\nExisting 1? %d", searchList(head, 1));
	printf("\nExisting 2? %d", searchList(head, 2));

	printf("\n");

	return 0;
}
