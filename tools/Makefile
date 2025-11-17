CC = gcc
CFLAGS = -Wall -Wextra -O2

all: validator

validator: validator.c
    $(CC) $(CFLAGS) validator.c -o validator

clean:
    rm -f validator

test: validator
    ./validator ../ornekler/

.PHONY: all clean test
