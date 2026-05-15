/*
 * hashtabletest.c - test program for CS50 hashtable module
 *
 * usage: read lines of text from stdin.
 *
 * This program is a "unit test" for the hashtable module.
 *
 * Megan Lu, COSC050, 26S
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "hashtable.h"
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

static bool testNew(hashtable_t** ht, const char* htName, const int numSlots);
static int testIterateCount(hashtable_t* ht, const char* htName, int expected);
static void testPrint(hashtable_t* ht, const char* htName);
static int testInsert(hashtable_t* ht, const char* htName, const char* key, void* item);
static void testFind(hashtable_t* ht, const char* htName, const char* key); 
static void pairPrint(FILE* fp, const char* key, void* item);
static void pairCount(void* arg, const char* key, void* item);
static void lineDelete(void* item);
static void lineEdit(void* arg, const char* key, void*item);
static void lineEmpty(void* arg, const char* key, void*item);

/**************** functions ****************/
int 
main(const int argc, const char* argv[])
{
    hashtable_t* ht1 = NULL; // one hashtable
    hashtable_t* ht2 = NULL; // another hashtable
    char* line = NULL;  // a line in the hashtable
    int lineCount = 0;  // number of lines put in the hashtable
    int htCount = 0;   // number of lines found in a hashtable

    printf("**************** hashtable test cases ****************\n");

    /* **************************************** */
    printf("\n****************************************\n");
    // `hashtable_new` test: Invalid number of slots
    bool created = testNew(&ht1, "ht1", 0);
    printf("Return value: %d\n", created);
    created = testNew(&ht1, "ht1", -5);
    printf("Return value: %d\n", created);
    created = testNew(&ht1, "ht1", 1);
    printf("Return value: %d\n", created);
    
    // `hashtable_new` test: Create a new hashtable
    if (!testNew(&ht1, "ht1", 10)) {
        // Failed to create new hashtable
        return 1;
    }
    // Check that hashtable is empty with `hashtable_print` and count
    // `hashtable_iterate` test: Empty hashtable
    htCount = testIterateCount(ht1, "ht1", 0);
    // `hashtable_print` test: Empty hashtable
    testPrint(ht1, "ht1");
    
    // `hashtable_new` test: Create a new table with a different number of slots
    if (!testNew(&ht2, "ht2", 100) == 1) {
        // Failed to create new hashtable
        return 1;
    }

    /* **************************************** */
    printf("\n****************************************\n");
    // `hashtable_insert` tests (all should return false):
    printf("\nTesting hashtable_insert (all should return false)...\n");
    
    // Null hashtable, good key and item
    testInsert(NULL, "(null)", "Dartmouth", "2028");
    
    // Good hashtable and item, null key
    testInsert(ht1, "ht1", NULL, "hi!");
    
    // Good hashtable and key, null item
    testInsert(ht1, "ht1", "key", NULL);

    htCount = testIterateCount(ht1, "ht1", 0);

    printf("\nTesting hashtable_insert (non-null parameters)...\n");
    // First insertion
    // Need to allocate memory for item to match other lines
    line = malloc(strlen("line") + 1);
    if (line != NULL) {
        strcpy(line, "line");
        testInsert(ht1, "ht1", "key", line);
        lineCount++;
    }
    // Repeat for other hashtable
    // Item pointers must be separate to prevent freeing twice when deleting
    line = malloc(strlen("line") + 1);
    if (line != NULL) {
        strcpy(line, "line");
        testInsert(ht2, "ht2", "key", line);
    }

    // `hashtable_iterate` test: One (key,item) pair in hashtable
    htCount = testIterateCount(ht1, "ht1", 1);
    htCount = testIterateCount(ht2, "ht2", htCount);

    // `hashtable_print` test: One (key,item) pair in hashtable
    testPrint(ht1, "ht1");
    testPrint(ht2, "ht2");

    /* `hashtable_insert` tests:
     * Normal insertion (all valid parameters) of a unique key (inserted)
     * Normal insertion (all valid parameters) of a duplicate key (not inserted)
     * Normal insertion (all valid parameters) of a unique key with the same hash (inserted)
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

            // Insert into first hashtable
            int inserted = testInsert(ht1, "ht1", key, line);

            // Copy line to a new buffer
            // Need to allocate memory for line to prevent freeing same pointer twice 
            // when deleting both hashtables
            char* copy = malloc(strlen(line) + 1);
            if (copy != NULL) {
                strcpy(copy, line);
                testInsert(ht2, "ht2", key, copy);
            }

            // If not inserted, free line and copy 
            // (will not be freed by set_delete later)
            if (!inserted) {
                free(line);
                free(copy);
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
        testInsert(ht1, "ht1", "unique key", copy);
        lineCount++;
    }

    // Repeat for ht2
    copy = malloc(strlen(line) + 1);
    if (copy != NULL) {
        strcpy(copy, line);
        testInsert(ht2, "ht2", "unique key", copy);
    }

    // `hashtable_iterate` test: 
    // Multiple (key,item) pairs in hashtable in different sets 
    // (different hashes) and valid function
    htCount = testIterateCount(ht1, "ht1", lineCount);
    htCount = testIterateCount(ht2, "ht2", htCount);
    
    // `hashtable_print` test: 
    // Multiple (key,item) pairs in hashtable in different sets (different hashes) 
    testPrint(ht1, "ht1");
    testPrint(ht2, "ht2");

    /* **************************************** */
    printf("\n****************************************\n");
    // `hashtable_find` tests:
    printf("\nTesting hashtable_find...\n");

    // Null hashtable
    testFind(NULL, "(null)", "key");

    // Null key
    testFind(ht1, "ht1", NULL);

    // Key found in hashtable (based on test.txt)
    testFind(ht1, "ht1", "unique key");
    testFind(ht2, "ht2", "unique key");
    // Key at different hashes in hashtable
    testFind(ht1, "ht1", "key");
    testFind(ht2, "ht2", "key");
    testFind(ht1, "ht1", "Hello");
    testFind(ht2, "ht2", "Hello");

    // Key not found in hashtable
    testFind(ht1, "ht1", "not in set");
    testFind(ht2, "ht2", "not in set");

    // Check that (key,item) pair was not removed from the hashtable 
    // with `hashtable_print` and count
    htCount = testIterateCount(ht1, "ht1", htCount);
    htCount = testIterateCount(ht2, "ht2", htCount);
    testPrint(ht1, "ht1");
    testPrint(ht2, "ht2");

    /* **************************************** */
    printf("\n****************************************\n");
    // `hashtable_print` tests:
    printf("\nTesting hashtable_print with null parameters...\n");

    // Null hashtable (prints (null))
    testPrint(NULL, "(null)");

    // Null fp (nothing printed)
    printf("\nPrinting to null fp...\n");
    hashtable_print(ht1, NULL, pairPrint);
    printf("\n");

    // Null itemprint (prints a hashtable with no items)
    printf("\nPrinting hashtable \"ht1\"with null itemprint function...\n");
    hashtable_print(ht1, stdout, NULL);
    printf("\n");

    /* **************************************** */
    printf("\n****************************************\n");
    // `hashtable_iterate` tests:
    printf("\nTesting hashtable_iterate...\n");

    // Null hashtable
    printf("\nIterating over null set to count items...\n");
    testIterateCount(NULL, "(null)", 0);

    // Null itemfunc
    printf("\nIterating over hashtable \"ht1\" with null itemfunc...\n");
    hashtable_iterate(ht1, NULL, NULL);

    // Multiple (key,item) pairs in hashtable 
    // and valid function that changes contents of the items
    // Check contents of the items are changed with `hashtable_print` 
    printf("\nIterating over hashtable \"ht1\" with itemfunc to capitalize first character in item...\n");
    printf("Before:\n");
    testPrint(ht1, "ht1");
    hashtable_iterate(ht1, NULL, lineEdit);
    printf("After:\n");
    testPrint(ht1, "ht1");

    // Repeat for ht2
    printf("\nIterating over hashtable \"ht2\" with itemfunc to capitalize first character in item...\n");
    printf("Before:\n");
    testPrint(ht2, "ht2");
    hashtable_iterate(ht2, NULL, lineEdit);
    printf("After:\n");
    testPrint(ht2, "ht2");

    // Check hashtable and its contents are not changed by this function with `hashtable_print` and an empty function
    // Use empty function
    printf("\nIterating over hashtable \"ht1\" with empty itemfunc...\n");
    printf("Before:\n");
    testPrint(ht1, "ht1");
    hashtable_iterate(ht1, NULL, lineEmpty);
    printf("After:\n");
    testPrint(ht1, "ht1");

    /* **************************************** */
    printf("\n****************************************\n");
    // `hashtable_delete` tests:
    printf("\nTesting hashtable_delete...\n");

    // Null hashtable
    printf("\nDeleting null hashtable...\n");
    hashtable_delete(NULL, lineDelete);

    // Hashtable with (key,item) pairs
    printf("\nDeleting hashtable \"ht1\" with (key,item) pairs...\n");
    hashtable_delete(ht1, lineDelete);
    printf("\nDeleting hashtable \"ht2\" with (key,item) pairs...\n");
    hashtable_delete(ht2, lineDelete);

    // Remake hashtable for next test
    testNew(&ht1, "ht1", 3);

    // Null itemdelete
    printf("\nDeleting hashtable \"ht1\" with null itemdelete function...\n");
    hashtable_delete(ht1, lineDelete);

    // Remake set for empty set
    testNew(&ht1, "ht1", 3);

    // Empty set
    printf("\nDeleting empty hashtable \"ht1\"...\n");
    hashtable_delete(ht1, lineDelete);
    
    printf("\n****************************************\n");

    return 0;
}

/**************** testNew ****************/
/* Helper function to create a set using hashtable_new.
 * 
 * Caller provides:
 *   A valid pointer to a hashtable_t pointer,
 *   the name of the hashtable, and the number of slots
 *   to be added.
 * We print:
 *   A line to stdout to show that hashtable_new is being tested.
 *   To stderr if hashtable_new fails.
 * We return:
 *   1 if the hashtable_new returned NULL.
 */
static bool 
testNew(hashtable_t** ht, const char* htName, const int numSlots)
{
    printf("Creating hashtable \"%s\" with %d slots with hashtable_new...\n", htName, numSlots);
    *ht = hashtable_new(numSlots);
    if (*ht == NULL)
    {
        fprintf(stderr, "hashtable_new failed for \"%s\"\n", htName);
        return false;
    }
    return true;
}

/**************** testIterateCount ****************/
/* Helper function to count the number of (key,item) pairs in a hashtable
 * using hashtable_iterate.
 * 
 * Caller provides:
 *   A valid pointer to hashtable_t, the name of the hashtable,
 *   and expected count (non-negative integer).
 * We print:
 *   The count and expected count.
 * We return:
 *   The hashtable count.
 */
static int 
testIterateCount(hashtable_t* ht, const char* htName, int expected)
{
    int count = 0;
    hashtable_iterate(ht, &count, pairCount);
    printf("Count of \"%s\": %d (expected count: %d)\n", htName, count, expected);
    return count;
}

/**************** testPrint ****************/
/* Helper function to print the hashtable using hashtable_print.
 * 
 * Caller provides:
 *   A valid pointer to hashtable_t and the name of the hashtable.
 * We print:
 *   A line to stdout to show that hashtable_print is being tested
 *   and the result of hashtable_print.
 */
static void 
testPrint(hashtable_t* ht, const char* htName)
{
    printf("Printing the hashtable \"%s\" with hashtable_print...\n", htName);
    hashtable_print(ht, stdout, pairPrint);
    printf("\n");
}

/**************** testInsert ****************/
/* Helper function to insert a (key,item) pair into a hashtable
 * using hashtable_insert.
 * 
 * Caller provides:
 *   A valid pointer to hashtable_t, the name of the hashtable, 
 *   a string key, and an item.
 * We print:
 *   A line to stdout to show that hashtable_insert is being tested
 *   and the return value of hashtable_insert.
 */
static int 
testInsert(hashtable_t* ht, const char* htName, const char* key, void* item)
{
    printf("Inserting ");
    pairPrint(stdout, key, item);
    printf(" to hashtable \"%s\" with hashtable_insert...\n", htName);
    int inserted = hashtable_insert(ht, key, item);
    printf("Return value: %d\n", inserted);
    return inserted;
}

/**************** testFind ****************/
/* Helper function to find a key in a hashtable
 * using hashtable_find.
 * 
 * Caller provides:
 *   A valid pointer to hashtable_t, the name of the hashtable,
 *   and a string key.
 * We print:
 *   A line to stdout to show that hashtable_find is being tested
 *   and the item located by hashtable_find in (key,item) pair form.
 */
static void 
testFind(hashtable_t* ht, const char* htName, const char* key)
{
    printf("Finding \"%s\" key in hashtable \"%s\" with hashtable_find...\n", key, htName);
    printf("(key,item): ");
    pairPrint(stdout, key, hashtable_find(ht, key));
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
