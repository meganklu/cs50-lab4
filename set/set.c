/* 
 * set.c - CS50 'set' module
 *
 * see set.h for more information.
 *
 * Megan Lu, COSC050, 26S
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "set.h"
#include "mem.h"

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/
typedef struct setnode {
	char* key; // string-type search key for this item
	void* item; // point to data for this item
	struct setnode* next; // link to next node
} setnode_t;

/**************** global types ****************/
typedef struct set {
	struct setnode* head; // head of the list of nodes in set
} set_t;

/**************** global functions ****************/
/* that is, visible outside this file */
/* see set.h for comments about exported functions */

/**************** local functions ****************/
/* not visible outside this file */
static setnode_t* setnode_new(const char* key, void* item);

/**************** set_new ****************/
/* see set.h for description */
set_t*
set_new(void)
{
	set_t* set = mem_malloc(sizeof(set_t));

	if (set == NULL) {
		return NULL; // error allocating set
	} else {
		// Initialize contents of set structure
		set->head = NULL;
		return set;
	}
}

/**************** set_insert ****************/
/* see set.h for description */
bool 
set_insert(set_t* set, const char* key, void* item)
{
	// Check if any parameter is NULL
	if (set == NULL || key == NULL || item == NULL) {
		return false;
	}

	// Search the set for the key
	if (set_find(set, key) != NULL) {
		// Return false if the key is already in the set
		return false;
	}

	// Allocate a new node to be added to the list
	setnode_t* new = setnode_new(key, item);

	if (new == NULL) {
		return false; // error allocating node
	}
	
	// Add new node to the head of the list
	new->next = set->head;
	set->head = new;

#ifdef TESTING
	mem_report(stdout, "After set_insert");
#endif
	
	return true;
}

/**************** setnode_new ****************/
/* Allocate and initialize a setnode */
static setnode_t*  // not visible outside this file
setnode_new(const char* key, void* item)
{
	setnode_t* node = mem_malloc(sizeof(setnode_t));

	if (node == NULL) {
		return NULL; // error allocating node
	}

	// Allocate memory for the string key
	// Length of string + 1 for null terminating character
	// (characters are 1 byte so number of characters is the number of bytes)
	char* copy = mem_malloc(strlen(key) + 1);

	if (copy == NULL) {
		return NULL; // error allocating key buffer
	}

	// Copy key string
	strcpy(copy, key);

	// Assign values to node
	node->key = copy;
	node->item = item;
	node->next = NULL;

	return node;
}	

/**************** set_find ****************/
/* see set.h for description */
void* 
set_find(set_t* set, const char* key)
{
	// Check if any parameter is NULL
	if (set == NULL || key == NULL) {
		return NULL;
	}

	// Loop through the list starting at the head 
	// until the key is found or the node is NULL
	for (setnode_t* current = set->head; 
		current != NULL; 
		current = current->next) {
		// Return the item if the key of current matches the search key
		if (strcmp(key, current->key)) {
			return current->item;
		}
	}

	// Exit the loop and key was not found, return NULL
	return NULL;
}

/**************** set_print ****************/
/* see set.h for description */
void 
set_print(set_t* set, FILE* fp, void (*itemprint)(FILE* fp, const char* key, void* item) )
{
	// Check if the file pointer is NULL
	// Print nothing if file pointer is NULL
	if (fp != NULL) {
		// Check if the set is NULL
		if (set != NULL) {
			fputc('{', fp);

			// Loop through all nodes in the list starting at the head 
			for (setnode_t* current = set->head; 
				current != NULL; 
				current = current->next) {
					// Print the node using the function parameter
					// Function prints (key,item)
					if (itemprint != NULL) {
						(*itemprint)(fp, current->key, current->item);
						fputc(',', fp);
					}
			}

			fputc('}', fp);
		} else {
			// Print (null) if set is NULL
			fputs("(null)", fp);
		}
	}
}

/**************** set_iterate ****************/
/* see set.h for description */
void 
set_iterate(set_t* set, void* arg, void (*itemfunc)(void* arg, const char* key, void* item) )
{
	// Check if parameters are NULL
	if (set != NULL && itemfunc != NULL) {
		// Loop through all nodes in the list starting at the head
		for (setnode_t* current = set->head; 
			current != NULL; 
			current = current->next) {
				// Call itemfunc on each node
				(*itemfunc)(arg, current->key, current->item);
		}
	}
}

/**************** set_delete ****************/
/* see set.h for description */
void
set_delete(set_t* set, void (*itemdelete)(void* item) )
{
	// Check if the set is NULL
	if (set != NULL) {
		// Loop through all nodes in the set starting at the head
		for (setnode_t* current = set->head; current != NULL; ) {
				// Check if the key is NULL
				if (current->key != NULL) {
					// Free the memory for the string representing the key
					mem_free(current->key);
				}

				// Check if the itemdelete function is NULL
				if (itemdelete != NULL) {
					// Delete the node's item if possible
					(*itemdelete)(current->item);
				}

				// Save the next node
				setnode_t* next = current->next;
				// Free the current node
				mem_free(current);
				// Move current to the next node
				current = next;
		}

		// Free the entire set
		mem_free(set);
	}

#ifdef MEMTEST
	mem_report(stdout, "End of set_delete");
#endif
}