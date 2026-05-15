# Lab 4: C Modules
Megan Lu, COSC050, 26S


### set

A `set` maintains an unordered collection of _(key,item)_ pairs; any given _key_ can only occur in the set once. 
It starts out empty and grows as the caller inserts new _(key,item)_ pairs. 
The caller can retrieve _items_ by asking for their _key_, but cannot remove or update pairs. 
_Items_ are distinguished by their _key_. 

### Usage

The *set* module, defined in `set.h` and implemented in `set.c`, implements a set of `void*` _items_ with `char*` (string-type) _keys_, and exports the following functions:

```c
set_t* set_new(void);
bool set_insert(set_t* set, const char* key, void* item);
void* set_find(set_t* set, const char* key);
void set_print(set_t* set, FILE* fp, void (*itemprint)(FILE* fp, const char* key, void* item) );
void set_iterate(set_t* set, void* arg, void (*itemfunc)(void* arg, const char* key, void* item) );
void set_delete(set_t* set, void (*itemdelete)(void* item) );
```

### Implementation

We implement this set as a linked list. 
The *set* itself is represented as a `struct set` containing a pointer to the head of the list; the head pointer is NULL when the set is empty.

Each node in the list is a `struct setnode`, a type defined internally to the module.
Each setnode includes a pointer to the `char* key`, a pointer to the `void* item`, and a pointer to the next setnode on the list.

To insert a new item in the set we create a new setnode to hold the `key` and `item`, and insert it at the head of the list.
The `key` string is copied for use by the set; thus, the caller is free to re-use or deallocate this string after insertion. 

To find an item from the set given a key with `set_find`, the list is traversed starting from the head. We return a pointer to the desired item, if found, and NULL if set is NULL, key is NULL, or key is not found. 
The item (node in the list) is *not* removed from the set.

The `set_print` method prints a little syntax around the list, and between items, but mostly calls the `itemprint` function on each key and item pair by scanning the linked list.

The `set_iterate` method calls the `itemfunc` function on each key and item pair by scanning the linked list.

The `set_delete` method scans the linked list, freeing the memory allocated for the key strings, calling the `itemdelete` function on each item, and freeing setnodes as it proceeds.
It concludes by freeing the `struct set`.

### Assumptions

No assumptions beyond those that are clear from the spec.

The `item` inserted cannot be NULL, and thus a NULL return from `set_find` must indicate error, an empty set, a NULL parameter `key`, or `key` not found, not a NULL `item` located at the `key`. 

Because of the semantics of a *set*, we have great freedom in our implementation.
Our linked-list approach actually implements a modified stack: `set_insert` pushes a new item on the stack, only if the item does not already exist.
The set does not have a function to pop the top item off the stack.
Other implementations are permitted, but this list approach is fast and easy to implement.
However, the computational complexity is not ideal as the list must be traversed with linear time complexity ($O(n)$) for each `set_insert` and `set_find`. A binary tree would make these processes more efficient with $ O(\log n) $ time complexity.

### Files
* `Makefile` - compilation procedure
* `set.h` - the interface
* `set.c` - the implementation
* `settest.c` - unit test driver
* `test.txt` - test data
* `testing.out` - result of `make test &> testing.out`

### Compilation

To compile, simply `make set.o`.

### Testing

The `settest.c` program reads lines from stdin and inserts each into a set as the item. 
The key is the first word in the line.
The test then searches for specific keys.
It tests a the error and edge cases described below.
Comments in `settest.c` show when each of these cases are tested.

`set_new` tests:
- Create a new set
    - Check that set is empty with `set_print` and count

`set_insert` tests:
- Null set, good key and item
- Good set and item, null key
- Good set and key, null item
- Normal insertion (all valid parameters) of a unique key (inserted)
- Normal insertion (all valid parameters) of a duplicate key (not inserted)
- Normal insertion (all valid parameters) of a unique key and duplicate item (inserted)

`set_find` tests:
- Null set
- Null key
- Key found in set
    - Key at head of the set
    - Key at the end of the set
    - Key in the middle of the set
    - Check that (key,item) pair was not removed from the set with `set_print` and count
- Key not found in set

`set_print` tests:
- Null set (prints (null))
- Null fp (nothing printed)
- Null itemprint (prints a set with no items)
- Empty set
- One (key,item) pair in set
- Multiple (key,item) pairs in set

*Note: The 'itemprint' function is responsible for printing (key,item) and handling null keys and items.*

`set_iterate` tests:
- Null set
- Null itemfunc
- One (key,item) pair in set
- Multiple (key,item) pairs in set and valid function
    - Check set and its contents are not changed by this function with `set_print` and an empty function
- Multiple (key,item) pairs in set and valid function that changes contents of the items
    - Check contents of the items are changed with `set_print` 

`set_delete` tests:
- Null set
- Null itemdelete
- Empty set
- Set with (key,item) pairs


To test, simply `make test`.
See `testing.out` for details of testing and an example test run.

Try testing with `MEMTEST` by editing Makefile to turn on that flag and then `make test`.

To test with valgrind, `make valgrind`.