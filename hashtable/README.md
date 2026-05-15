# Lab 4: C Modules
Megan Lu, COSC050, 26S


### hashtable

A `hashtable` is a set of _(key,item)_ pairs. It acts just like a set, but is far more efficient for large collections.
Like a `set`, it maintains an unordered collection of _(key,item)_ pairs; any given _key_ can only occur in the set once. 
It starts out empty and grows as the caller inserts new _(key,item)_ pairs. 
The caller can retrieve _items_ by asking for their _key_, but cannot remove or update pairs. 
_Items_ are distinguished by their _key_. 

### Usage

The *hashtable* module, defined in `hashtable.h` and implemented in `hashtable.c`, implements a hashtable of `void*` _items_ with `char*` (string-type) _keys_, and exports the following functions:

```c
hashtable_t* hashtable_new(const int num_slots);
bool hashtable_insert(hashtable_t* ht, const char* key, void* item);
void* hashtable_find(hashtable_t* ht, const char* key);
void hashtable_print(hashtable_t* ht, FILE* fp, void (*itemprint)(FILE* fp, const char* key, void* item));
void hashtable_iterate(hashtable_t* ht, void* arg, void (*itemfunc)(void* arg, const char* key, void* item) );
void hashtable_delete(hashtable_t* ht, void (*itemdelete)(void* item) );
```

### Implementation

We implement this hashtable as an array of sets (set pointers). 
The *hashtable* itself is represented as a `struct hashtable` containing an array of pointers to sets (a pointer to the first index in the array where a pointer to a set is stored) and the number of slots.
The slots array will always be initialized when a hashtable is created (not a NULL pointer).
However, the set for each hash slot is initialized as NULL until at least one (key,item) pair is added to that set.

The *set* module is implemented separately as a linked list. 
The *hashtable* only relies on the interface of *set* and does not account for its implementation (e.g., the internal types of *set* are not used).
There are no local types of functions defined internally to the *hashtable* module.

The module relies on the hash function defined in `hash.h` to calculate the hash of keys.

To insert a new item in the hashtable we calculate the hash of the key.
If the set pointer for that hash (index in the slots array) is NULL, which occurs if the hash slot has not been used yet, a new set is created with `set_new`. 
The `key` and `item` are inserted into the appropriate set, whether it was just created or previously initialized, using `set_insert`.
`set_insert` will only insert the new pair if the set was allocated without error (non-NULL value) and the key has not been added yet.
The `set_insert` method will also copy the `key` string for use by the set and hashtable, in general.
Thus, the caller is free to re-use or deallocate this string after insertion. 

To find an item from the hashtable given a key with `hashtable_find`, the hash of the key is calculated.
Then, the hash slot set is searched for the key using `set_find`.
`set_find` will check if the set at the hash slot has been initialized (i.e., is not NULL anymore).
The `hashtable_find` method returns the `set_find` return value.
`set_find` returns a pointer to the desired item, if found, and NULL if set is NULL or key is not found (key is not NULL if it is passed into `set_find`). 
The item is *not* removed from the set or hashtable.

The `hashtable_print` method prints each slot on a separate line with the slot number first.
It calls the `set_print` function on each set in the slots array if the set is not NULL.
If the set is NULL, it prints an empty set.
The `set_print` method is implemented to print key and item pairs in a comma-separated list format. 

The `hashtable_iterate` method calls the `set_iterate` function on each slot in the hashtable, passing the `itemfunc` and `arg` to this method.
The `set_iterate` method handles NULL set and NULL `itemfunc`.

The `hashtable_delete` method loops through the slots array, freeing each set by calling the `set_delete` method.
`set_delete` will free the memory allocated for the key strings, call the `itemdelete` function on each item, free setnodes, and free the set structure.
This method does not do anything if the set is NULL.
`hashtable_delete` concludes by freeing the `struct hashtable`.

### Assumptions

No assumptions beyond those that are clear from the spec.

The `item` inserted cannot be NULL, and thus a NULL return from `hashtable_find` must indicate error, an empty hashtable, a NULL parameter `key`, or `key` not found, not a NULL `item` located at the `key`. 

This *hashtable* handles collisions with chaining.
Specifically, the separate chains are linked lists due to the implementation of *set*.

### Files
* `Makefile` - compilation procedure
* `hashtable.h` - the interface
* `hashtable.c` - the implementation
* `hash.h` - the interface for the hash function
* `hash.c` - the implementation of the hash function
* `hashtabletest.c` - unit test driver
* `test.txt` - test data
* `testing.out` - result of `make test &> testing.out`

### Compilation

To compile, simply `make hashtable.o`.

### Testing

The `hashtabletest.c` program reads lines from stdin and inserts each into a set as the item. 
The key is the first word in the line.
The test then searches for specific keys.
Each (key,item) pair is added to two hashtables with different numbers of slots.
It tests the error and edge cases described below.
Comments in `hashtabletest.c` show where each of these cases are tested.

`hashtable_new` tests:
- Create a new hashtable
    - Check that hashtable is empty with `hashtable_print` and count
- Create a new table with a different number of slots
- Invalid number of slots
- One slot (edge case)

`hashtable_insert` tests:
- Null hashtable, good key and item
- Good hashtable and item, null key
- Good hashtable and key, null item
- Normal insertion (all valid parameters) of a unique key (inserted)
- Normal insertion (all valid parameters) of a duplicate key (not inserted)
- Normal insertion (all valid parameters) of a unique key with the same hash (inserted)
- Normal insertion (all valid parameters) of a unique key and duplicate item (inserted)

`hashtable_find` tests:
- Null hashtable
- Null key
- Key found in hashtable
    - Key at different hashes in hashtable
    - Check that (key,item) pair was not removed from the hashtable with `hashtable_print` and count
- Key not found in hashtable

`hashtable_print` tests:
- Null hashtable (prints (null))
- Null fp (nothing printed)
- Null itemprint (prints a hashtable with no items)
- Empty hashtable
- One (key,item) pair in hashtable
- Multiple (key,item) pairs in hashtable in different sets (different hashes) 

*Note: The 'itemprint' function is responsible for printing (key,item) and handling null keys and items.*

`hashtable_iterate` tests:
- Null hashtable
- Null itemfunc
- Empty hashtable
- One (key,item) pair in hashtable
- Multiple (key,item) pairs in hashtable in different sets (different hashes) and valid function
    - Check hashtable and its contents are not changed by this function with `hashtable_print` and an empty function
- Multiple (key,item) pairs in hashtable and valid function that changes contents of the items
    - Check contents of the items are changed with `hashtable_print` 

`hashtable_delete` tests:
- Null hashtable
- Null itemdelete
- Empty hashtable
- Hashtable with (key,item) pairs


To test, simply `make test`.
See `testing.out` for details of testing and an example test run.

Try testing with `MEMTEST` by editing Makefile to turn on that flag and then `make test`.

To test with valgrind, `make valgrind`.