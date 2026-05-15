# Lab 4: C Modules
Megan Lu, COSC050, 26S


### counters

A **counter set** (`counters`) is a set of counters, each distinguished by an integer _key_. 
It's a set - each _key_ can only occur once in the set - and it tracks a _counter_ for each key. 
It starts empty. 
Each time `counters_add` is called on a given _key_, the corresponding _counter_ is incremented. 
The current _counter_ value can be retrieved by asking for the relevant _key_.

### Usage

The *counters* module, defined in `counters.h` and implemented in `counters.c`, implements a set of integer counters with `int` _keys_ (where _keys_ must be non-negative), and exports the following functions:

```c
counters_t* counters_new(void);
int counters_add(counters_t* ctrs, const int key);
int counters_get(counters_t* ctrs, const int key);
bool counters_set(counters_t* ctrs, const int key, const int count);
void counters_print(counters_t* ctrs, FILE* fp);
void counters_iterate(counters_t* ctrs, void* arg, void (*itemfunc)(void* arg, const int key, const int count));
void counters_delete(counters_t* ctrs);
```

### Implementation

We implement this counter set as a linked list. 
The *counters* itself is represented as a `struct counters` containing a pointer to the head of the list; the head pointer is NULL when the counter set is empty.

Each node in the list is a `struct countersnode`, a type defined internally to the module.
Each countersnode includes an integer key (non-negative), an integer counter, and a pointer to the next countersnode on the list.

To insert a new counter in the counter set we create a new countersnode to hold the `key` and `count`, and insert it at the head of the list. 
The `counters_add` method and the `counters_set` method will create a new countersnode if the key does not exist in the counter set yet.
The list will be traversed to search for the key before creating a new countersnode.
The counter of the new node will be initialized differently depending on the method: `counters_add` will give the new node a count of 1 and `counters_set` will set the count to the parameter given.
The `counters_add` method and the `counters_set` method will update an existing count if the key already exists. 
`counters_add` will increment the count by 1. 
`counters_set` will change the count to the parameter given.

To get the current count of a given a key with `counters_get`, the list is traversed starting from the head. 
We return the count, if found, and 0 if counters is NULL, key is negative, key is not found, or the count is 0. 
The counter (node in the list) is *not* removed from counters.

The `counters_print` method prints a little syntax around the list, and between counters.
It scans the linked list and prints the key=counter pairs.

The `counters_iterate` method calls the `itemfunc` function on each key and counter pair by scanning the linked list.

The `counters_delete` method scans the linked list, freeing the memory allocated for the setnodes as it proceeds.
It concludes by freeing the `struct counters`.

### Assumptions

No assumptions beyond those that are clear from the spec.

The value of `count` cannot be 0 after `counters_add`.
A return of 0 from `counters_add` must indicate either error, NULL counters, or invalid key, not a key with a count of 0.

The value of `count` can be 0 if the key is added or updated using `counters_set`.
Thus, a return of 0 from `counters_get` can mean that counters is NULL, the key is negative, the key is not found, or the count is 0.

Because of the semantics of *counters*, we have great freedom in our implementation.
Our linked-list approach actually implements a modified stack: `counters_add` and `counters_set` pushes a new item on the stack, only if the key does not already exist in counters.
The counter set does not have a function to pop the top item off the stack.
Other implementations are permitted, but this list approach is fast and easy to implement.
However, the computational complexity is not ideal as the list must be traversed with linear time complexity ($O(n)$) for each call of `counters_add`, `counters_set`, and `counters_get` with valid parameters. A binary tree would make these processes more efficient with $ O(\log n) $ time complexity.

### Files
* `Makefile` - compilation procedure
* `counters.h` - the interface
* `counters.c` - the implementation
* `counterstest.c` - unit test driver
* `test.txt` - test data
* `testing.out` - result of `make test &> testing.out`

### Compilation

To compile, simply `make counters.o`.

### Testing

The `counterstest.c` program reads numbers from stdin on separate lines.
Each number is added to two counter sets.
One counter set uses `counters_add` to add numbers.
The other counter set is modified using `counters_set` where the number is both the key and the counter value.
The test then gets the count of specific keys.
It tests a the error and edge cases described below.
Comments in `counterstest.c` show where each of these cases are tested.

`counters_new` tests:
- Create a new counter set
    - Check that set is empty with `counters_print` and count

`counters_add` tests:
- Null counters, good key
- Good counters, invalid key (negative number)
- Normal addition (all valid parameters) of a unique key (inserted with a count of 1)
    - Key is 0 (edge case)
- Normal addition (all valid parameters) of a duplicate key (count incremented)

`counters_get` tests:
- Null counters
- Invalid key (negative number)
- Key found in counters
    - Key at head of counters
    - Key at the end of counters
    - Key in the middle of counters
    - Check that (key,item) pair was not removed from counters with `counters_print` and count
- Key is not found in counters

`counters_set` tests:
- Null counters
- Invalid key (negative number)
- Invalid count (negative number)
- Normal setting (all valid parameters) of a unique key (added to counters with provided count value)
    - Key is 0 (edge case)
    - Count is 0 (edge case)
- Normal setting (all valid parameters) of a duplicate key (count updated)

`counters_print` tests:
- Null counters (prints (null))
- Null fp (nothing printed)
- Empty counter set
- One counter in counter set
- Multiple counters in counter set

`counters_iterate` tests:
- Null counters
- Null itemfunc
- Empty counter set
- One counter in counter set
- Multiple counters in counter set and valid function
    - Check counters and its contents are not changed by this function with `set_print` 

`counters_delete` tests:
- Null counters
- Empty counters
- Multiple counters in counter set


To test, simply `make test`.
See `testing.out` for details of testing and an example test run.

Try testing with `MEMTEST` by editing Makefile to turn on that flag and then `make test`.

To test with valgrind, `make valgrind`.