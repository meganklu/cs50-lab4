/*
 * settest.c - test program for CS50 set module
 *
 * usage: read lines of text from stdin.
 *
 * This program is a "unit test" for the set module.
 *
 * CS50, April 2019, 2021
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "set.h"
#include "file.h"

static void lineprint(FILE* fp, const char* key, void* item);
static void linedelete(void* item);
static void paircount(void* arg, const char* key, void* item);
static void editline(void* arg, const char* key, void*item);

/* **************************************** */
int 
main(const int argc, const char* argv[])
{
    set_t* set = NULL; // a set
    char* line = NULL;  // a line in the set
    int linecount = 0;  // number of lines put in the set
    int setcount = 0;   // number of lines found in a set

    printf("**************** set test cases ****************\n");

    /* **************************************** */
    printf("\n****************************************\n");
    // `set_new` test:

    // Create a new set
    printf("Create a set...\n");
    set = set_new();
    if (set == NULL)
    {
        fprintf(stderr, "set_new failed for set\n");
        return 1;
    } else {
        // Check that set is empty with `set_print` and count

        // `set_iterate` test:
        // Empty set
        set_iterate(set, &setcount, paircount);
        printf("Count: %d\n", setcount);

        // `set_print` test:
        // Empty set
        printf("The empty set:\n");
        set_print(set, stdout, lineprint);
        printf("\n");
    }

    /* **************************************** */
    printf("\n****************************************\n");
    // `set_insert` tests (all return false):
    printf("\nTesting set_insert (all return false)...\n");
    
    // Null set, good key and item
    printf("\nTest with null set, good key and item...\n");
    printf("Return value: %d\n", set_insert(NULL, "Dartmouth", "2028"));
    
    // Good set and item, null key
    printf("\nTest with good set and item, null key...\n");
    printf("Return value: %d\n", set_insert(set, NULL, "hi!"));
    
    // Good set and key, null item
    printf("\nTest with good set and key, null item...\n");
    printf("Return value: %d\n", set_insert(set, "key", NULL));

    printf("\nCount (should be zero): ");
    setcount = 0;
    set_iterate(set, &setcount, paircount);
    printf("%d\n", setcount);

    printf("\nTesting set_insert (non-null parameters)...\n");
    linecount = 0;
    // First insertion
    // Need to allocate memory for item to match other lines
    line = malloc(strlen("line") + 1);
    if (line != NULL) {
        strcpy(line, "line");
        printf("\nFirst insertion: adding (%s,\"%s\")...\n", "key", line);
        printf("Return value: %d\n", set_insert(set, "key", line));
        linecount++;
    }

    // `set_print` test:
    // One (key,item) pair in set
    printf("\nThe set with one (key,item) pair:\n");
    set_print(set, stdout, lineprint);
    printf("\n");

    // `set_iterate` test:
    // One (key,item) pair in set
    printf("\nCount (should be %d): ", linecount);
    setcount = 0;
    set_iterate(set, &setcount, paircount);
    printf("%d\n", setcount);

    // `set_insert` tests:
    // Read lines from stdin
    // Normal insertion (all valid parameters) of a unique key (inserted)
    // Normal insertion (all valid parameters) of a duplicate key (not inserted)
    // (above test cases depend on input file/stdin used)
    while (!feof(stdin))
    {
        line = file_readLine(stdin);
        if (line != NULL)
        {
            // Parse first word to use as key
            // Copy to a new string (not modifying original line string)
            // Buffer in stack (not used later so does not need to be in the heap)
            char buffer[strlen(line) + 1];
            strcpy(buffer, line);
            char* key = strtok(buffer, " ");
            if (key == NULL) {
                // Empty string if the return of strtok is null 
                // (empty string or no characters other than the delimiter)
                key = "";
            }

            // Insert into set
            printf("\nAdding (%s,\"%s\")...\n", key, line);
            int inserted = set_insert(set, key, line);
            printf("Return value: %d\n", inserted);
            linecount++;

            // If not inserted, free line (will not be freed by set_delete later)
            if (!inserted) {
                free(line);
            }
        }
    }

    // Normal insertion (all valid parameters) of a unique key and duplicate item (inserted)
    // Uses previously save line (key must be unique, change depending on input)
    // Need to allocate memory for line to prevent freeing same pointer twice when deleting set
    char* copy = malloc(strlen(line) + 1);
    if (copy != NULL) {
        strcpy(copy, line);
        printf("\nAdding (%s,\"%s\")...\n", "unique key", copy);
        printf("Return value: %d\n", set_insert(set, "unique key", copy));
        linecount++;
    }

    // `set_print` test:
    // Multiple (key,item) pairs in set
    printf("\nThe set (before set_iterate):\n");
    set_print(set, stdout, lineprint);
    printf("\n");

    printf("\nCount (should be less than or equal to %d): ", linecount);
    setcount = 0;
    // `set_iterate` test:
    // Multiple (key,item) pairs in set and valid function
    set_iterate(set, &setcount, paircount);
    printf("%d\n", setcount);
    // Check set and its contents are not changed by this function with `set_print` 
    printf("\nThe set (after set_iterate):\n");
    set_print(set, stdout, lineprint);
    printf("\n");
    
    /* **************************************** */
    printf("\n****************************************\n");
    // `set_find` tests:
    printf("\nTesting set_find...\n");

    // Null set
    printf("\nFinding in null set...\n");
    printf("(key,item): ");
    lineprint(stdout, "key", set_find(NULL, "key"));
    printf("\n");

    // Null key
    printf("\nFinding null key...\n");
    printf("(key,item): ");
    lineprint(stdout, NULL, set_find(set, NULL));
    printf("\n");

    // Key found in set (based on test.txt)
    // Key at head of the set
    printf("\nFinding \"unique key\" key...\n");
    printf("(key,item): ");
    lineprint(stdout, "unique key", set_find(set, "unique key"));
    printf("\n");

    // Key at the end of the set
    printf("\nFinding \"key\" key...\n");
    printf("(key,item): ");
    lineprint(stdout, "key", set_find(set, "key"));
    printf("\n");

    // Key in the middle of the set
    printf("\nFinding \"Hello\" key...\n");
    printf("(key,item): ");
    lineprint(stdout, "Hello", set_find(set, "Hello"));
    printf("\n");

    // Check that (key,item) pair was not removed from the set with `set_print` and count
    printf("\nCount (should be still %d): ", setcount);
    setcount = 0;
    set_iterate(set, &setcount, paircount);
    printf("%d\n", setcount);
    printf("\nThe set (after set_find):\n");
    set_print(set, stdout, lineprint);
    printf("\n");

    // Key not found in set
    printf("\nFinding \"not in set\" key...\n");
    printf("(key,item): ");
    lineprint(stdout, "not in set", set_find(set, "not in set"));
    printf("\n");

    /* **************************************** */
    printf("\n****************************************\n");
    // `set_print` tests:
    printf("\nTesting set_print with null parameters...\n");

    // Null set (prints (null))
    printf("\nPrinting null set...\n");
    set_print(NULL, stdout, lineprint);
    printf("\n");

    // Null fp (nothing printed)
    printf("\nPrinting to null fp...\n");
    set_print(set, NULL, lineprint);
    printf("\n");

    // Null itemprint (prints a set with no items)
    printf("\nPrinting with null itemprint function...\n");
    set_print(set, stdout, NULL);
    printf("\n");

    /* **************************************** */
    printf("\n****************************************\n");
    // `set_iterate` tests:
    printf("\nTesting set_iterate...\n");

    // Null set
    printf("\nIterating over null set to count items...\n");
    setcount = 0;
    set_iterate(NULL, &setcount, paircount);
    printf("%d\n", setcount);

    printf("\nIterating with null itemfunc...\n");
    set_iterate(set, &setcount, NULL);

    // Multiple (key,item) pairs in set and valid function that changes contents of the items
    printf("\nIterating with itemfunc to capitalize first character in item...\n");
    printf("The set (before set_iterate):\n");
    set_print(set, stdout, lineprint);
    printf("\n");
    set_iterate(set, NULL, editline);
    printf("The set (after set_iterate):\n");
    set_print(set, stdout, lineprint);
    printf("\n");

    /* **************************************** */
    printf("\n****************************************\n");
    // `set_delete` tests:
    printf("\nTesting set_delete...\n");

    // Null set
    printf("\nDeleting null set...\n");
    set_delete(NULL, linedelete);

    // Set with (key,item) pairs
    printf("\nDeleting set with (key,item) pairs...\n");
    set_delete(set, linedelete);

    // Remake set
    set = set_new();

    // Null itemdelete
    printf("\nDeleting with null itemdelete function...\n");
    set_delete(set, NULL);

    // Remake set for empty set
    set = set_new();

    // Empty set
    printf("\nDeleting empty set...\n");
    set_delete(set, linedelete);
    
    printf("\n****************************************\n");

    return 0;
}

/* Count the non-null (key,item) pairs in the set.
 * note here we don't care what kind of item is in set.
 */
static void
paircount(void* arg, const char* key, void* item)
{
    int* npairs = arg;

    if (npairs != NULL && key != NULL && item != NULL)
    {
        (*npairs)++;
    }
}

/* Print the (key,item) pair in parantheses and separated by a comma.
 * Format the line (item), in quotes.
 * (null) will be printed as the key or item if it is a NULL pointer
 */
static void 
lineprint(FILE* fp, const char* key, void* item)
{
    fputc('(', fp);

    // Check if key is NULL
    if (key == NULL)
    {
        fprintf(fp, "(null)");
    }
    else
    {
        fprintf(fp, "%s", key);
    }

    fputc(',', fp);

    char* line = item;
    if (line == NULL)
    {
        fprintf(fp, "(null)");
    }
    else
    {
        fprintf(fp, "\"%s\"", line);
    }

    fputc(')', fp);
}

// Delete a line
static void 
linedelete(void* item)
{
    if (item != NULL)
    {
        free(item);
    }
}

// Capitalizes the first character in the line
static void 
editline(void* arg, const char* key, void* item)
{
    char* line = item;
    // Check if there is at least 1 character in the line
    if (item != NULL && strlen(item) > 0) {
        // Extract the first character of the line
        char first = line[0];
        // Convert to capital letter 
        first = toupper(first);
        // Set first character of line to uppercase character
        line[0] = first;
    }
}
