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

We implement this set as a 
