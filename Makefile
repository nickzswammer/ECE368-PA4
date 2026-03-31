# make testall tests all cases
# Test cases where k = 1
# Test edge cases where u only have one element / 1 node
#
# make memory runs valgrind on all test cases

# Variables

DEBUG = -g
OPTIMIZE = -O3
CFLAGS_COMMON = -std=c99 -pedantic -Wvla -Wall -Wshadow

GCC = gcc
SRCS = $(PA).c tree.c
OBJS = $(SRCS:%.c=%.o)
PA = pa4

default: $(PA)

$(PA): CFLAGS = $(CFLAGS_COMMON) -O3
$(PA): $(OBJS)
	$(GCC) $(CFLAGS) $(OBJS) -o $(PA)

debug: CFLAGS = $(CFLAGS_COMMON) -g -O0
debug: clean $(OBJS)
	$(GCC) $(CFLAGS) $(OBJS) -o $(PA)

%.o: %.c
	$(GCC) $(CFLAGS) -c $< -o $@

zip:
	zip pa4.zip *.c *.h Makefile

clean:
	rm -rf $(OBJS) $(PA) my* *.zip *.b outfile* out/* error.log results.txt

