/*
 * settest.c - test program for CS50 set module
 *
 * usage: read lines of text from stdin.
 *
 * This program is a "unit test" for the set module.
 *
 * Megan Lu, COSC050, 26S
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "set.h"
#include "file.h"

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/
/* none */

/**************** global types ****************/
/* none */

/**************** global functions ****************/
/* that is, visible outside this file */
/* none other than main() function */

/**************** local functions ****************/
/* not visible outside this file */

static void testNew(set_t** set);
static int testIterateCount(set_t* set, int expected);
static void testPrint(set_t* set);
static int testInsert(set_t* set, const char* key, void* item);
static void testFind(set_t* set, const char* key); 
static void pairPrint(FILE* fp, const char* key, void* item);
static void pairCount(void* arg, const char* key, void* item);
static void lineDelete(void* item);
static void lineEdit(void* arg, const char* key, void*item);
static void lineEmpty(void* arg, const char* key, void*item);

/**************** functions ****************/
int 
main(const int argc, const char* argv[])
{
    set_t* set = NULL; // a set
    char* line = NULL;  // a line in the set
    int lineCount = 0;  // number of lines put in the set
    int setCount = 0;   // number of lines found in a set

    printf("**************** set test cases ****************\n");

    /* **************************************** */
    printf("\n****************************************\n");
    // `set_new` test: Create a new set
    testNew(&set);
    // Check that set is empty with `set_print` and count
    // `set_iterate` test: Empty set
    setCount = testIterateCount(set, 0);
    // `set_print` test: Empty set
    testPrint(set);

    /* **************************************** */
    printf("\n****************************************\n");
    // `set_insert` tests (all should return false):
    printf("\nTesting set_insert (all should return false)...\n");
    
    // Null set, good key and item
    testInsert(NULL, "Dartmouth", "2028");
    
    // Good set and item, null key
    testInsert(set, NULL, "hi!");
    
    // Good set and key, null item
    testInsert(set, "key", NULL);

    setCount = testIterateCount(set, 0);

    printf("\nTesting set_insert (non-null parameters)...\n");
    // First insertion
    // Need to allocate memory for item to match other lines
    line = malloc(strlen("line") + 1);
    if (line != NULL) {
        strcpy(line, "line");
        testInsert(set, "key", line);
        lineCount++;
    }

    // `set_print` test: One (key,item) pair in set
    testPrint(set);

    // `set_iterate` test: One (key,item) pair in set
    setCount = testIterateCount(set, 1);

    /* `set_insert` tests:
     * Normal insertion (all valid parameters) of a unique key (inserted)
     * Normal insertion (all valid parameters) of a duplicate key (not inserted)
     * 
     * Read lines from stdin
     * (above test cases depend on input file/stdin used)
     */
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
            int inserted = testInsert(set, key, line);

            // If not inserted, free line (will not be freed by set_delete later)
            if (!inserted) {
                free(line);
            } else {
                lineCount++;
            }
        }
    }

    // Normal insertion (all valid parameters) of a unique key and duplicate item (inserted)
    // Uses previously save line (key must be unique, change depending on input)
    // Need to allocate memory for line to prevent freeing same pointer twice when deleting set
    char* copy = malloc(strlen(line) + 1);
    if (copy != NULL) {
        strcpy(copy, line);
        testInsert(set, "unique key", copy);
        lineCount++;
    }

    // `set_iterate` test: Multiple (key,item) pairs in set and valid function
    setCount = testIterateCount(set, lineCount);
    
    // `set_print` test: Multiple (key,item) pairs in set
    testPrint(set);

    /* **************************************** */
    printf("\n****************************************\n");
    // `set_find` tests:
    printf("\nTesting set_find...\n");

    // Null set
    testFind(NULL, "key");

    // Null key
    testFind(set, NULL);

    // Key found in set (based on test.txt)
    // Key at head of the set
    testFind(set, "unique key");

    // Key at the end of the set
    testFind(set, "key");

    // Key in the middle of the set
    testFind(set, "Hello");

    // Key not found in set
    testFind(set, "not in set");

    // Check that (key,item) pair was not removed from the set with `set_print` and count
    setCount = testIterateCount(set, setCount);
    testPrint(set);

    /* **************************************** */
    printf("\n****************************************\n");
    // `set_print` tests:
    printf("\nTesting set_print with null parameters...\n");

    // Null set (prints (null))
    printf("\nPrinting null set...\n");
    set_print(NULL, stdout, pairPrint);
    printf("\n");

    // Null fp (nothing printed)
    printf("\nPrinting to null fp...\n");
    set_print(set, NULL, pairPrint);
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
    testIterateCount(NULL, 0);

    // Null itemfunc
    printf("\nIterating with null itemfunc...\n");
    set_iterate(set, &setCount, NULL);

    // Multiple (key,item) pairs in set and valid function that changes contents of the items
    printf("\nIterating with itemfunc to capitalize first character in item...\n");
    printf("Before:\n");
    testPrint(set);
    set_iterate(set, NULL, lineEdit);
    printf("After:\n");
    testPrint(set);

    // Check set and its contents are not changed by this function with `set_print` and an empty function
    // Use empty function
    printf("\nIterating with empty itemfunc...\n");
    printf("Before:\n");
    testPrint(set);
    set_iterate(set, NULL, lineEmpty);
    printf("After:\n");
    testPrint(set);

    /* **************************************** */
    printf("\n****************************************\n");
    // `set_delete` tests:
    printf("\nTesting set_delete...\n");

    // Null set
    printf("\nDeleting null set...\n");
    set_delete(NULL, lineDelete);

    // Set with (key,item) pairs
    printf("\nDeleting set with (key,item) pairs...\n");
    set_delete(set, lineDelete);

    // Remake set for next test
    testNew(&set);

    // Null itemdelete
    printf("\nDeleting with null itemdelete function...\n");
    set_delete(set, NULL);

    // Remake set for empty set
    testNew(&set);

    // Empty set
    printf("\nDeleting empty set...\n");
    set_delete(set, lineDelete);
    
    printf("\n****************************************\n");

    return 0;
}

/**************** testNew ****************/
/* Helper function to create a set using set_new.
 * 
 * Caller provides:
 *   A valid pointer to a set_t pointer.
 * We print:
 *   A line to stdout to show that set_new is being tested.
 *   To stderr if set_new fails.
 * We exit:
 *   If the set_new returned NULL.
 */
static void
testNew(set_t** set)
{
    printf("Creating a set with set_new...\n");
    *set = set_new();
    if (*set == NULL)
    {
        fprintf(stderr, "set_new failed for set\n");
        exit(1);
    }
}

/**************** testIterateCount ****************/
/* Helper function to count the number of (key,item) pairs in a set
 * using set_iterate.
 * 
 * Caller provides:
 *   A valid pointer to set_t and expected count (non-negative integer).
 * We print:
 *   The count and expected count.
 * We return:
 *   The set count.
 */
static int 
testIterateCount(set_t* set, int expected)
{
    int count = 0;
    set_iterate(set, &count, pairCount);
    printf("Count: %d (expected count: %d)\n", count, expected);
    return count;
}

/**************** testPrint ****************/
/* Helper function to print the set using set_print.
 * 
 * Caller provides:
 *   A valid pointer to set_t.
 * We print:
 *   A line to stdout to show that set_print is being tested
 *   and the result of set_print.
 */
static void 
testPrint(set_t* set)
{
    printf("Printing the set with set_print...\n");
    set_print(set, stdout, pairPrint);
    printf("\n");
}

/**************** testInsert ****************/
/* Helper function to insert a (key,item) pair into a set
 * using set_insert.
 * 
 * Caller provides:
 *   A valid pointer to set_t, string key, and item.
 * We print:
 *   A line to stdout to show that set_insert is being tested
 *   and the return value of set_insert.
 */
static int 
testInsert(set_t* set, const char* key, void* item)
{
    printf("Inserting ");
    pairPrint(stdout, key, item);
    if (set == NULL) {
        printf(" into (null)");
    }
    printf("...\n");
    int inserted = set_insert(set, key, item);
    printf("Return value: %d\n", inserted);
    return inserted;
}

/**************** testFind ****************/
/* Helper function to find a key in a set
 * using set_find.
 * 
 * Caller provides:
 *   A valid pointer to set_t and string key.
 * We print:
 *   A line to stdout to show that set_find is being tested
 *   and the item located by set_find in (key,item) pair form.
 */
static void 
testFind(set_t* set, const char* key)
{
    printf("Finding \"%s\" key", key);
    if (set == NULL) {
        printf(" in (null)");
    }
    printf("...\n");
    printf("(key,item): ");
    pairPrint(stdout, key, set_find(set, key));
    printf("\n");
}

/**************** pairPrint ****************/
/* itemprint function for the lines of text in the set.
 * 
 * Caller provides:
 *   A valid file pointer, string key, and line (item).
 * We print:
 *   The (key,item) pair in parantheses and separated by a comma
 *   to the provided fp.
 *   The line (item) is formatted in quotes.
 *   (null) will be printed as the key/item if it is a NULL pointer.
 */
static void 
pairPrint(FILE* fp, const char* key, void* item)
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

/**************** pairCount ****************/
/* itemfunc function to count the (key,item) pairs in the set
 * using set_iterate.
 * 
 * Caller provides:
 *   A pointer to an integer for the counter as arg,
 *   a key, and an item.
 * We do:
 *   Count the non-null (key,item) pairs in the set.
 * Note:
 *   We don't care what kind of item is in set.
 */
static void
pairCount(void* arg, const char* key, void* item)
{
    int* npairs = arg;

    if (npairs != NULL && key != NULL && item != NULL)
    {
        (*npairs)++;
    }
}

/**************** lineDelete ****************/
/* itemdelete function free the lines stored as items in the set.
 * 
 * Caller provides:
 *   A pointer to a line stored in the heap 
 *   (intially created using malloc).
 * We do:
 *   Free the memory pointed to by the line if the item is not NULL.
 */
static void 
lineDelete(void* item)
{
    if (item != NULL)
    {
        free(item);
    }
}

/**************** lineEdit ****************/
/* itemfunc function to capitalize the first character in the line
 * (item) using set_iterate.
 * 
 * Caller provides:
 *   An item.
 * We do:
 *   Capitalize the first character in the line.
 * Note:
 *   An argument arg is not needed/used.
 */
static void 
lineEdit(void* arg, const char* key, void* item)
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

/**************** lineEmpty ****************/
/* itemfunc function that is empty.
 * 
 * We guarantee:
 *   None of the argument parameters will be changed.
 */
static void 
lineEmpty(void* arg, const char* key, void*item)
{
}
