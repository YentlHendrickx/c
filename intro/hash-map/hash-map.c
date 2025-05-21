#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node {
  char *key;
  char *value;
  struct Node *next;
};

void setNode(struct Node *node, char *key, char *value) {
  node->key = key;
  node->value = value;
  node->next = NULL;
  return;
}

// LIFO stack implementation
struct HashMap {
  int elementCount, capacity;

  struct Node **nodes;
};

int hashFunction(struct HashMap *map, char *key) {
  int sum = 0, factor = 31;

  // Totally didn't steal this hash function from the internet
  for (int i = 0; i < (int)strlen(key); i++) {
    // sum + (ascii value char * (primeNumber ^ x))
    // where x = 1, 2, 3....n
    sum = ((sum % map->capacity) + (((int)key[i]) * factor) % map->capacity) %
          map->capacity;

    // factor = factor * primeNumber
    // number...(primeNumber ^ x)
    factor = ((factor % __INT16_MAX__) * (31 % __INT16_MAX__)) % __INT16_MAX__;
  }

  return sum;
}

void initializeMap(struct HashMap *map) {
  map->elementCount = 0;
  map->capacity = 50;

  // Allocate memory for the nodes, which is an array of pointers to Node
  // Size = capacity * sizeof(struct Node *)
  map->nodes = (struct Node **)malloc(sizeof(struct Node *) * map->capacity);
  return;
}

struct HashMap *resizeMap(struct HashMap *map, int newCapacity) {
  // Allocate memory for the new nodes
  struct Node **newNodes =
      (struct Node **)malloc(sizeof(struct Node *) * newCapacity);

  // Initialize the new nodes to NULL
  for (int i = 0; i < newCapacity; i++) {
    newNodes[i] = NULL;
  }

  // Rehash all the existing nodes into the new nodes
  for (int i = 0; i < map->capacity; i++) {
    struct Node *node = map->nodes[i];
    while (node != NULL) {
      int index = hashFunction(map, node->key) % newCapacity;
      struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
      setNode(newNode, node->key, node->value);
      newNode->next = newNodes[index];
      newNodes[index] = newNode;
      node = node->next;
    }
  }

  // Free the old nodes
  free(map->nodes);

  // Update the map with the new nodes and capacity
  map->nodes = newNodes;
  map->capacity = newCapacity;

  return map;
}

void insert(struct HashMap *map, char *key, char *value) {
  int index = hashFunction(map, key);
  struct Node *node = (struct Node *)malloc(sizeof(struct Node));

  setNode(node, key, value);

  // Check if we need to resize the map
  if (map->elementCount >= map->capacity) {
    map = resizeMap(map, map->capacity * 2); // Double the capacity
  }

  // Check if bucket index is empty -> zero collisions
  if (map->nodes[index] == NULL) {
    map->nodes[index] = node;
  } else {
    // COLLISION -> create new node at head of list
    node->next = map->nodes[index];
    map->nodes[index] = node;
  }

  return;
}

char *get(struct HashMap *map, char *key) {
  // find bucket
  int index = hashFunction(map, key);
  // find node
  struct Node *node = map->nodes[index];

  // traverse linked list and find key
  while (node != NULL) {
    if (strcmp(node->key, key) == 0) {
      return node->value;
    }
    node = node->next;
  }

  return NULL; // Key not found
}

void removeValue(struct HashMap *map, char *key) {
  int index = hashFunction(map, key);

  struct Node *node = map->nodes[index];

  // traverse linked list and find key
  while (node != NULL) {
    if (strcmp(node->key, key) == 0) {
      struct Node *temp = node;
      node = node->next;
      free(temp->key);
      free(temp->value);
      free(temp);
      map->elementCount--;
      return;
    }
    node = node->next;
  }
}

int main(void) {
  struct HashMap *map = (struct HashMap *)malloc(sizeof(struct HashMap));
  initializeMap(map);

  int testKeyCount = 2;
  char **testKeys = (char **)malloc(sizeof(char *) * testKeyCount);

  for (int i = 0; i < testKeyCount; i++) {
    testKeys[i] = (char *)malloc(sizeof(char) * 10);
    sprintf(testKeys[i], "key%d", i);
  }

  for (int i = 0; i < testKeyCount; i++) {
    char *key = testKeys[i];
    char *value = (char *)malloc(sizeof(char) * 10);
    sprintf(value, "value%d", i);
    insert(map, key, value);
  }

  for (int i = 0; i < testKeyCount; i++) {
    char *key = testKeys[i];
    char *value = get(map, key);
    printf("Key: %s, Value: %s\n", key, value);
  }

  // Next remove first element
  removeValue(map, testKeys[0]);

  for (int i = 0; i < testKeyCount; i++) {
    char *key = testKeys[i];
    char *value = get(map, key);
    printf("Key: %s, Value: %s\n", key, value);
  }

  return 0;
}
