/*
 * counterstest.c - test program for CS50 counters module
 *
 * usage: read numbers from stdin, each on one line
 *
 * This program is a "unit test" for the counters module.
 *
 * Megan Lu, COSC050, 26S
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "counters.h"
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

static void testNew(counters_t** ctrs, const char* ctrsName);
static int testIterateCount(counters_t* ctrs, const char* ctrsName, int expected);
static void testPrint(counters_t* ctrs, const char* ctrsName);
static int testAdd(counters_t* ctrs, const char* ctrsName, const int key);
static void testSet(counters_t* ctrs, const char* ctrsName, const int key, const int count);
static void testGet(counters_t* ctrs, const char* ctrsName, const int key);
static void countersCount(void* arg, const int key, const int count);

/**************** functions ****************/
int 
main(const int argc, const char* argv[])
{
    counters_t* ctrs1 = NULL; // one counter set (uses counters_add)
    counters_t* ctrs2 = NULL; // another counter set (uses counters_set)
    int keyCount = 0;  // number of keys put in ctrs
    int ctrsCount = 0;   // number of key=count pairs found in the counterset

    printf("**************** counters test cases ****************\n");

    /* **************************************** */
    printf("\n****************************************\n");
    // `counters_new` test: Create a new counter set
    testNew(&ctrs1, "ctrs1");
    // Check that set is empty with `counters_print` and count
    // `counters_iterate` test: Empty counter set
    ctrsCount = testIterateCount(ctrs1, "ctrs1", 0);
    // `counters_print` test: Empty set
    testPrint(ctrs1, "ctrs1");
    // Create a second counter set
    testNew(&ctrs2, "ctrs2");

    /* **************************************** */
    printf("\n****************************************\n");
    // `counters_add` tests (all should return 0):
    printf("\nTesting counters_add (all should return 0)...\n");
    
    // Null counters, good key
    testAdd(NULL, "(null)", 6);
    
    // Good counters, invalid key (negative number)
    testAdd(ctrs1, "ctrs1", -1);

    ctrsCount = testIterateCount(ctrs1, "ctrs1", 0);

    /* **************************************** */
    printf("\n****************************************\n");
    // `counters_set` tests (all should return false):
    printf("\nTesting counters_set (all should return false)...\n");
    
    // Null counters
    testSet(NULL, "(null)", 6, 3);
    
    // Invalid key (negative number)
    testSet(ctrs2, "ctrs2", -3, 12);

    // Invalid count (negative number)
    testSet(ctrs2, "ctrs2", 2, -10);

    ctrsCount = testIterateCount(ctrs2, "ctrs2", 0);

    /* **************************************** */
    printf("\n****************************************\n");
    // `counters_add` and `counters_set` tests (valid parameters):
    printf("\nTesting counters_add and counters_set (valid parameters)...\n");
    
    // First insertion
    // `counters_add` test: Normal addition (all valid parameters) 
    // of a unique key (inserted with a count of 1)
    // Key is 0 (edge case)
    testAdd(ctrs1, "ctrs1", 0);

    // First insertion
    // `counters_set` test: Normal setting (all valid parameters) 
    // of a unique key (added to counters with provided count value)
    // Key is 0 (edge case)
    // Count is 0 (edge case)
    testSet(ctrs2, "ctrs2", 0, 0);

    keyCount++;

    // `counters_iterate` test: One counter in counter set
    ctrsCount = testIterateCount(ctrs1, "ctrs1", 1);
    ctrsCount = testIterateCount(ctrs2, "ctrs2", 1);

    // `counter_print` test: One counter in counter set
    testPrint(ctrs1, "ctrs1");
    testPrint(ctrs2, "ctrs2");

    // `counters_add` test: Normal addition (all valid parameters) 
    // of a duplicate key (count incremented)
    testAdd(ctrs1, "ctrs1", 0);

    // `counters_set` test: Normal setting (all valid parameters) 
    // of a duplicate key (count updated)
    testSet(ctrs2, "ctrs2", 0, 5);

    ctrsCount = testIterateCount(ctrs1, "ctrs1", 1);
    ctrsCount = testIterateCount(ctrs2, "ctrs2", 1);
    testPrint(ctrs1, "ctrs1");
    testPrint(ctrs2, "ctrs2");

    // Read numbers on separate lines from stdin
    while (!feof(stdin))
    {
        char* line = file_readLine(stdin);
        if (line != NULL)
        {
            // Convert the string to an int
            int number = atoi(line);

            // Free the line (file_readLine uses malloc)
            free(line);

            // Add to ctrs1 using counters_add
            if (testAdd(ctrs1, "ctrs1", number) == 1) {
                // Count is 1 if new key is added
                keyCount++;
            }
            
            // Set in ctrs2 using counters_set
            testSet(ctrs2, "ctrs2", number, number);
        }
    }

    // `counters_iterate` tests: Multiple counters in counter set 
    // and valid function
    ctrsCount = testIterateCount(ctrs1, "ctrs1", keyCount);
    ctrsCount = testIterateCount(ctrs2, "ctrs2", keyCount);
    
    // `counters_print` test: Multiple counters in counter set
    testPrint(ctrs1, "ctrs1");
    testPrint(ctrs2, "ctrs2");

    /* **************************************** */
    printf("\n****************************************\n");
    // `counters_get` tests:
    printf("\nTesting counters_get...\n");

    // Null counters
    testGet(NULL, "(null)", 0);

    // Invalid key (negative number)
    testGet(ctrs1, "ctrs1", -5);

    // Key found in counters (based on test.txt)
    // Key at head of the set
    testGet(ctrs1, "ctrs1", 200);

    // Key at the end of the set
    testGet(ctrs1, "ctrs1", 0);

    // Key in the middle of the set
    testGet(ctrs1, "ctrs1", 6);

    // Key not found in set
    testGet(ctrs1, "ctrs1", 1000);

    // Check that (key,item) pair was not removed from the set with `set_print` and count
    ctrsCount = testIterateCount(ctrs1, "ctrs1", ctrsCount);
    testPrint(ctrs1, "ctrs1");

    /* **************************************** */
    printf("\n****************************************\n");
    // `counters_print` tests:
    printf("\nTesting set_print with null parameters...\n");

    // Null counters (prints (null))
    testPrint(NULL, "(null)");

    // Null fp (nothing printed)
    printf("\nPrinting ctrs1 to null fp...\n");
    counters_print(ctrs1, NULL);
    printf("\n");

    /* **************************************** */
    printf("\n****************************************\n");
    // `counters_iterate` tests:
    printf("\nTesting counters_iterate...\n");

    // Null counters
    testIterateCount(NULL, "(null)", 0);

    printf("\nIterating over ctrs1 with null itemfunc...\n");
    counters_iterate(ctrs1, NULL, NULL);

    // Check counters and its contents are not changed by this function 
    // with `set_print` 
    printf("Before:\n");
    testPrint(ctrs1, "ctrs1");
    testIterateCount(ctrs1, "ctrs1", ctrsCount);
    printf("After:\n");
    testPrint(ctrs1, "ctrs1");

    /* **************************************** */
    printf("\n****************************************\n");
    // `counters_delete` tests:
    printf("\nTesting counters_delete...\n");

    // Null counters
    printf("\nDeleting null counters...\n");
    counters_delete(NULL);

    // Multiple counters in counter set
    printf("\nDeleting \"ctrs1\"...\n");
    counters_delete(ctrs1);

    printf("\nDeleting \"ctrs2\"...\n");
    counters_delete(ctrs2);
    printf("\n");

    // Remake counters for next test
    testNew(&ctrs1, "ctrs1");

    // Empty counters
    printf("\nDeleting empty counter set (\"ctrs1\")...\n");
    counters_delete(ctrs1);
    
    printf("\n****************************************\n");

    return 0;
}

/**************** testNew ****************/
/* Helper function to create a counter set using counters_new.
 * 
 * Caller provides:
 *   A valid pointer to a counters_t pointer
 *   and the name of the counter set.
 * We print:
 *   A line to stdout to show that counters_new is being tested.
 *   To stderr if counters_new fails.
 * We exit:
 *   If the counters_new returned NULL.
 */
static void 
testNew(counters_t** ctrs, const char* ctrsName)
{
    printf("Creating counters \"%s\" with counters_new...\n", ctrsName);
    *ctrs = counters_new();
    if (*ctrs == NULL)
    {
        fprintf(stderr, "counters_new failed for \"%s\"\n", ctrsName);
        exit(1);
    }
}

/**************** testIterateCount ****************/
/* Helper function to count the number of key,counter pairs
 * in a counter set using counters_iterate.
 * 
 * Caller provides:
 *   A valid pointer to counters_t, the name of the counters,
 *   and the expected count (non-negative integer).
 * We print:
 *   The count and expected count.
 * We return:
 *   The counters count.
 */
static int 
testIterateCount(counters_t* ctrs, const char* ctrsName, int expected)
{
    int count = 0;
    counters_iterate(ctrs, &count, countersCount);
    printf("Count of \"%s\": %d (expected count: %d)\n", ctrsName, count, expected);
    return count;
}

/**************** testPrint ****************/
/* Helper function to print the counters using counters_print.
 * 
 * Caller provides:
 *   A valid pointer to counters_t and the name of the counters.
 * We print:
 *   A line to stdout to show that counters_print is being tested
 *   and the result of counters_print.
 */
static void 
testPrint(counters_t* ctrs, const char* ctrsName)
{
    printf("Printing the counters \"%s\" with counters_print...\n", ctrsName);
    counters_print(ctrs, stdout);
    printf("\n");
}

/**************** testAdd ****************/
/* Helper function to add a key into a counterset
 * using counters_add.
 * 
 * Caller provides:
 *   A valid pointer to counters_t, the name of the counters,
 *   and an integer key.
 * We print:
 *   A line to stdout to show that counters_add is being tested
 *   and the return value of counters_add.
 * We return:
 *   The return value of counters_add.
 */
static int 
testAdd(counters_t* ctrs, const char* ctrsName, const int key)
{
    printf("Adding %d to counters \"%s\" with counters_add...\n", key, ctrsName);
    int count = counters_add(ctrs, key);
    printf("Return value: %d\n", count);
    return count;
}

/**************** testSet ****************/
/* Helper function to set a key with a count in counters
 * using counters_set.
 * 
 * Caller provides:
 *   A valid pointer to counters_t, the name of the counters,
 *   an integer key, and integer count.
 * We print:
 *   A line to stdout to show that counters_set is being tested
 *   and the return value of counters_set.
 */
static void 
testSet(counters_t* ctrs, const char* ctrsName, const int key, const int count)
{
    printf("Setting %d=%d in counters \"%s\" with counters_set...\n", key, count, ctrsName);
    printf("Return value: %d\n", counters_set(ctrs, key, count));
}

/**************** testGet ****************/
/* Helper function to get the count of a key in
 * counters using counters_get.
 * 
 * Caller provides:
 *   A valid pointer to counters_t, the name of the counters,
 *   and an integer key.
 * We print:
 *   A line to stdout to show that counters_get is being tested
 *   and the count returned by the function.
 */
static void 
testGet(counters_t* ctrs, const char* ctrsName, const int key)
{
    printf("Getting count of key %d in counters \"%s\" with counters_get...\n", key, ctrsName);
    printf("Count of %d: %d\n", key, counters_get(ctrs, key));
}

/**************** countersCount ****************/
/* itemfunc function to count the key=count pairs in counters
 * using counters_iterate.
 * 
 * Caller provides:
 *   A pointer to an integer for the counter as arg,
 *   a key, and a count.
 * We do:
 *   Count the valid key=count pairs in counters.
 *   (Valid if neither integer value is negative.)
 */
static void 
countersCount(void* arg, const int key, const int count)
{
    int* npairs = arg;

    if (npairs != NULL && key >= 0 && count >= 0)
    {
        (*npairs)++;
    }
}
