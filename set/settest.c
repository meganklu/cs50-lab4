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
#include "set.h"
#include "file.h"

static void lineprint(FILE *fp, const char *key, void *item);
static void linedelete(void *item);
static void paircount(void *arg, const char *key, void *item);

/* **************************************** */
int main()
{
    set_t *set1 = NULL; // one set
    set_t *set2 = NULL; // another set
    char *line = NULL;  // a line in the bag
    int linecount = 0;  // number of lines put in the bag
    int setcount = 0;   // number of lines found in a bag

    printf("Create first set...\n");
    set1 = set_new();
    if (set1 == NULL)
    {
        fprintf(stderr, "set_new failed for set1\n");
        return 1;
    }

    printf("\nTest with null set, good key and item...\n");
    set_insert(NULL, "Dartmouth");
    printf("Test with null item...\n");
    bag_insert(bag1, NULL);
    printf("test with null bag, null item...\n");
    bag_insert(NULL, NULL);

    printf("\nCount (should be zero): ");
    bagcount = 0;
    bag_iterate(bag1, &bagcount, itemcount);
    printf("%d\n", bagcount);

    printf("\nTesting bag_insert...\n");
    // read lines from stdin
    namecount = 0;
    while (!feof(stdin))
    {
        name = file_readLine(stdin);
        if (name != NULL)
        {
            bag_insert(bag1, name);
            namecount++;
        }
    }

    printf("\nCount (should be %d): ", namecount);
    bagcount = 0;
    bag_iterate(bag1, &bagcount, itemcount);
    printf("%d\n", bagcount);

    printf("\nThe bag:\n");
    bag_print(bag1, stdout, nameprint);
    printf("\n");

    printf("\nMove items to a new bag...\n");
    bag2 = bag_new();
    if (bag2 == NULL)
    {
        fprintf(stderr, "bag_new failed for bag2\n");
        return 2;
    }

    while ((name = bag_extract(bag1)) != NULL)
    {
        bag_insert(bag2, name);
    }

    printf("\nThe old bag:\n");
    printf("Count (should be zero): ");
    bagcount = 0;
    bag_iterate(bag1, &bagcount, itemcount);
    printf("%d\n", bagcount);
    bag_print(bag1, stdout, nameprint);
    printf("\n");

    printf("\nThe new bag:\n");
    printf("Count (should be %d): ", namecount);
    bagcount = 0;
    bag_iterate(bag2, &bagcount, itemcount);
    printf("%d\n", bagcount);
    bag_print(bag2, stdout, nameprint);
    printf("\n");

    printf("\ndelete the bags...\n");
    bag_delete(bag1, namedelete);
    bag_delete(bag2, namedelete);

    return 0;
}

/* Count the non-null (key,item) pairs in the set.
 * note here we don't care what kind of item is in set.
 */
static void
paircount(void *arg, const char *key, void *item)
{
    int *npairs = arg;

    if (npairs != NULL && key != NULL && item != NULL)
    {
        (*npairs)++;
    }
}

/* Print the (key,item) pair in parantheses and separated by a comma.
 * Format the line (item), in quotes.
 * (null) will be printed as the key or item if it is a NULL pointer
 */
void lineprint(FILE *fp, const char *key, void *item)
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

    char *line = item;
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
void linedelete(void *item)
{
    if (item != NULL)
    {
        mem_free(item);
    }
}
