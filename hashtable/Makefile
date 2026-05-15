# Makefile for 'hashtable' module
#
# Megan Lu, COSC050, 26S

OBJS = hashtabletest.o hashtable.o hash.o ../set/set.o ../lib/file.o ../lib/mem.o 
LIBS =

# uncomment the following to turn on verbose memory logging
TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I../lib -I../set
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

hashtabletest: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

hashtabletest.o: hashtable.h ../lib/file.h
hashtable.o: hashtable.h hash.h ../set/set.h ../lib/mem.h
../lib/file.o: ../lib/file.h
../lib/mem.o: ../lib/mem.h
../set/set.o: ../set/set.h ../lib/mem.h

.PHONY: test valgrind clean

# expects a file `test.txt` to exist; it can contain any text.
test: hashtabletest test.txt
	./hashtabletest < test.txt

# expects a file `test.txt` to exist; it can contain any text.
valgrind: hashtabletest test.txt
	$(VALGRIND) ./hashtabletest < test.txt

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f hashtabletest
	rm -f core
