/* 
 * hashtable.c - CS50 'hashtable' module
 *
 * see hashtable.h for more information.
 *
 * Megan Lu, COSC050, 26S
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "hashtable.h"
#include "hash.h"
#include "set.h"
#include "mem.h"

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/
/* none */

/**************** global types ****************/
typedef struct hashtable {
	set_t** slots; // array of pointers to sets
    int numSlots; // number of slots to be used for the hashtable (must be > 0)
} hashtable_t;

/**************** global functions ****************/
/* that is, visible outside this file */
/* see set.h for comments about exported functions */

/**************** local functions ****************/
/* not visible outside this file */
/* none */

/**************** hashtable_new ****************/
/* see hashtable.h for description */
hashtable_t* 
hashtable_new(const int num_slots)
{
    // Check if number of slots is valid
    if (num_slots <= 0) {
        return NULL;
    }

	hashtable_t* ht = mem_malloc(sizeof(hashtable_t));

	if (ht == NULL) {
		return NULL; // error allocating hashtable
	}
	
    // Initialize contents of hashtable structure
    // Allocate memory for array of pointers to sets of size num_slots
    ht->slots = mem_malloc(sizeof(set_t*) * num_slots);
    ht->numSlots = num_slots;

    if (ht->slots == NULL) {
        // Free ht pointer (was successful before)
        mem_free(ht);
        return NULL; // error allocating slots
    }

    // Set each set pointed to in slots to NULL initially
    // Will allocate memory for the set when needed
    // Prevents allocating memory for a set that is empty and unused
    for (int i = 0; i < num_slots; i++) {
        ht->slots[i] = NULL;
    }

#ifdef MEMTEST
	mem_report(stdout, "After hashtable_new");
#endif

	return ht;
}

/**************** hashtable_insert ****************/
/* see hashtable.h for description */
bool 
hashtable_insert(hashtable_t* ht, const char* key, void* item)
{
	// Check if any parameter is NULL
	if (ht == NULL || key == NULL || item == NULL) {
		return false;
	}

    // Calculate the hash of the key
    int hash = hash_jenkins(key, ht->numSlots);

    // Check if the slot is a NULL pointer
    // This will only occur if the key is not in the hashtable yet
    // (set at the slot would exist if the key was already present)
    if (ht->slots[hash] == NULL) {
        // Initialze a new set
        ht->slots[hash] = set_new();
    }

    /* Add to the set at the corresponding slot
     * set_insert will search for the key before adding to the set
     * and will return false if the key is already in the hashtable.
     * Since the key will always be in the same slot of the hashtable,
     * only this set needs to be searched.
     * 
     * set_insert will also check if the set is NULL 
     * (error allocating set above).
     * 
     * The key string will be copied into the set 
     * (and thus the hashtable) by set_insert
     */
    bool added = set_insert(ht->slots[hash], key, item);

    // set_insert returns false if key already exists or set is NULL
    if (!added) {
        return false;
    }

#ifdef MEMTEST
    // Only report memory usage if insertion was successful
	mem_report(stdout, "After hashtable_insert");
#endif
	
	return true;
}

/**************** hashtable_find ****************/
/* see hashtable.h for description */
void* 
hashtable_find(hashtable_t* ht, const char* key)
{
	// Check if any parameter is NULL
	if (ht == NULL || key == NULL) {
		return NULL;
	}

    // Calculate the hash of the key
    int hash = hash_jenkins(key, ht->numSlots);

    /* set_find will check if set at the hash slot has been initialized
     * (pointer in slots points to non-NULL value).
     * The method will return false if the set is NULL
     * or the key is not found.
     */
    return set_find(ht->slots[hash], key);
}

/**************** hashtable_print ****************/
/* see hashtable.h for description */
void 
hashtable_print(hashtable_t* ht, FILE* fp, void (*itemprint)(FILE* fp, const char* key, void* item))
{
	// Check if the file pointer is NULL
	// Print nothing if file pointer is NULL
	if (fp != NULL) {
		// Check if the hashtable is NULL
		if (ht != NULL) {
		    // Loop through each slot in the hashtable
            for (int i = 0; i < ht->numSlots; i++) {
                // Print the hash slot number
                fprintf(fp, "slot %d: ", i);

                // Check if the slot has a NULL set
                set_t* set = ht->slots[i];
                if (set == NULL) {
                    // Print empty set (set_print would print (null))
                    fputs("{}", fp);
                } else {
                    // Use set_print to list the (key,item) pairs in that slot
                    // set_print will print a set with no items if NULL itemprint
                    set_print(set, fp, itemprint);
                }
                
                // New line for next hash slot
                fputc('\n', fp);
            }
		} else {
			// Print (null) if hashtable is NULL
			fputs("(null)", fp);
		}
	}
}

/**************** hashtable_iterate ****************/
/* see hashtable.h for description */
void  
hashtable_iterate(hashtable_t* ht, void* arg, void (*itemfunc)(void* arg, const char* key, void* item) )
{
	// Check if parameters are NULL
	if (ht != NULL && itemfunc != NULL) {
        // Loop through each slot in the hashtable
        for (int i = 0; i < ht->numSlots; i++) {
            // Iterate through set in slot with set_iterate
            // set_iterate will do nothing if set is NULL
            // (empty slot, not initialized yet by hashtable_insert)
            set_iterate(ht->slots[i], arg, itemfunc);
        }
	}
}

/**************** hashtable_iterate ****************/
/* see hashtable.h for description */
void 
hashtable_delete(hashtable_t* ht, void (*itemdelete)(void* item) )
{
    // Check if the hashtable is NULL
	if (ht != NULL) {
        // Loop through each slot in the hashtable
        for (int i = 0; i < ht->numSlots; i++) {
            /* Delete set in slot with set_delete
             * set_delete will do nothing if set is NULL 
             * (empty slot, uninitialized)
             * All memory associated with the slot will be freed 
             * (setnodes, copy of key string, items with itemdelete, set itself)
             * set_delete checks if itemdelete is NULL 
             * (only applies it to items if it is non-NULL)
             */
            set_delete(ht->slots[i], itemdelete);
        }

        // Free the slots array
        mem_free(ht->slots);

        // Free the entire hashtable
		mem_free(ht);
    }

#ifdef MEMTEST
	mem_report(stdout, "End of hashtable_delete");
#endif
}
