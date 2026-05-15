/* 
 * counters.c - CS50 'counters' module
 *
 * see counters.h for more information.
 *
 * Megan Lu, COSC050, 26S
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "counters.h"
#include "mem.h"

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/
typedef struct countersnode {
	int key; // integer key (non-negative)
	int count; // counter for the key
	struct countersnode* next; // link to next node
} countersnode_t;

/**************** global types ****************/
typedef struct counters {
	struct countersnode* head; // head of the list of nodes in the counter set
} counters_t;

/**************** global functions ****************/
/* that is, visible outside this file */
/* see counters.h for comments about exported functions */

/**************** local functions ****************/
/* not visible outside this file */
static countersnode_t* countersnode_new(const int key);
static countersnode_t* countersnode_find(counters_t* ctrs, const int key);

/**************** counters_new ****************/
/* see counters.h for description */
counters_t* 
counters_new(void)
{
	counters_t* counters = mem_malloc(sizeof(counters_t));

	if (counters == NULL) {
		return NULL; // error allocating counters
	} else {
		// Initialize contents of counters structure
		counters->head = NULL;

#ifdef MEMTEST
		mem_report(stdout, "After counters_new");
#endif

		return counters;
	}
}

/**************** counters_add ****************/
/* see counters.h for description */
int 
counters_add(counters_t* ctrs, const int key)
{
    // Check if parameters are valid 
    // (invalid if ctrs is NULL or key is negative)
    if (ctrs == NULL || key < 0) {
        return 0;
    }

    // Locate the node with the key
    countersnode_t* node = countersnode_find(ctrs, key);

    // Check if a node returned is NULL
    if (node == NULL) {
        // Allocate a new node with the key to be added to the list
        node = countersnode_new(key);

        if (node == NULL) {
            return 0; // error allocating node
        }

        // Add new node to the head of the list
        node->next = ctrs->head;
        ctrs-> head = node;

#ifdef MEMTEST
	    mem_report(stdout, "After counters_add");
#endif
    }

    // Increment counter in node (pre-increment; increment then return count)
    return ++(node->count);
}

/**************** countersnode_new ****************/
/* Allocate and initialize a countersnode. 
 *
 * Caller provides:
 *   A valid key (non-negative integer).
 * We return:
 *   A pointer to a new countersnode. 
 * We guarantee:
 *   The countersnode has the key provided
 *   and its next points to NULL.
 *   The counter is initially set to 0. 
 * Caller is responsible for:
 *   Later calling counters_delete() to free the pointer's memory.
 * Note:
 *   The function does not validate the value of key.
 */
static countersnode_t* // not visible outside this file
countersnode_new(const int key)
{
	countersnode_t* node = mem_malloc(sizeof(countersnode_t));

	if (node == NULL) {
		return NULL; // error allocating node
	}

	// Assign values to node
    // Integer key will be stored as its own copy 
    // (shallow copy for primitive type)
	node->key = key; 
	node->count = 0;
	node->next = NULL;

	return node;
}

/**************** countersnode_find ****************/
/* Find the node in counters with the given key.
 *
 * Caller provides:
 *   A valid counters and key (non-negative integer).
 * We return:
 *   A pointer to a countersnode with the key.
 *   NULL if counters or key is NULL or the key is not in counters.
 * We guarantee:
 *   The counter set is not changed by the function.
 * Note:
 *   The function does not check the value of ctrs or key.
 */
static countersnode_t* // not visible outside this file
countersnode_find(counters_t* ctrs, const int key)
{
	// Loop through the list starting at the head 
	// until the key is found or the node is NULL
	for (countersnode_t* current = ctrs->head; 
		current != NULL; 
		current = current->next) {
		// Return the node if the key of current matches the search key
		if (key == current->key) {
			return current;
		}
	}

	// Exit the loop and key was not found, return NULL
	return NULL;
}

/**************** counters_get ****************/
/* see counters.h for description */
int 
counters_get(counters_t* ctrs, const int key)
{
    // Check if parameters are valid 
    // (invalid if ctrs is NULL or key is negative)
    if (ctrs == NULL || key < 0) {
        return 0;
    }

    // Find the node with the key
    countersnode_t* node = countersnode_find(ctrs, key);

    // Check if node is NULL
    if (node == NULL) {
        return 0;
    }

    // Return the count stored by the node
    return node->count;
}

/**************** counters_set ****************/
/* see counters.h for description */
bool 
counters_set(counters_t* ctrs, const int key, const int count)
{
    // Check if parameters are valid 
    // (invalid if ctrs is NULL, key is negative, or count is negative)
    if (ctrs == NULL || key < 0 || count < 0) {
        return false;
    }

    // Locate the node with the key
    countersnode_t* node = countersnode_find(ctrs, key);

    // Check if a node returned is NULL
    if (node == NULL) {
        // Allocate a new node with the key to be added to the list
        node = countersnode_new(key);

        if (node == NULL) {
            return false; // error allocating node
        }

        // Add new node to the head of the list
        node->next = ctrs->head;
        ctrs-> head = node;

#ifdef MEMTEST
	    mem_report(stdout, "After counters_set");
#endif
    }

    // Change the count of the node
    node->count = count;
    return true;
}

/**************** counters_print ****************/
/* see counters.h for description */
void 
counters_print(counters_t* ctrs, FILE* fp)
{
    // Check if the file pointer is NULL
	// Print nothing if file pointer is NULL
	if (fp != NULL) {
		// Check if ctrs is NULL
		if (ctrs != NULL) {
			fputc('{', fp);

			// Loop through all nodes in the list starting at the head 
			for (countersnode_t* current = ctrs->head; 
				current != NULL; 
				current = current->next) {
					// Print the key=counter pair and comma
                    fprintf(fp, "%d=%d,", current->key, current->count); 
			}

			fputc('}', fp);
		} else {
			// Print (null) if ctrs is NULL
			fputs("(null)", fp);
		}
	}
}

/**************** counters_iterate ****************/
/* see counters.h for description */
void 
counters_iterate(counters_t* ctrs, void* arg, void (*itemfunc)(void* arg, const int key, const int count))
{
    // Check if parameters are NULL
	if (ctrs != NULL && itemfunc != NULL) {
        // Loop through all nodes in the list starting at the head
		for (countersnode_t* current = ctrs->head; 
			current != NULL; 
			current = current->next) {
				// Call itemfunc on each node
				(*itemfunc)(arg, current->key, current->count);
		}
    }
}

/**************** counters_delete ****************/
/* see counters.h for description */
void 
counters_delete(counters_t* ctrs)
{
    // Check if ctrs is NULL
	if (ctrs != NULL) {
		// Loop through all nodes in the list starting at the head
		for (countersnode_t* current = ctrs->head; current != NULL; ) {
				// Save the next node
				countersnode_t* next = current->next;
				// Free the current node
				mem_free(current);
				// Move current to the next node
				current = next;
		}

		// Free the entire counter sert
		mem_free(ctrs);
	}

#ifdef MEMTEST
	mem_report(stdout, "End of counters_delete");
#endif
}
