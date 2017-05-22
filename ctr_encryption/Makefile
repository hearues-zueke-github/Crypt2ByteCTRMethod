CC=gcc
CFLAGS=-Wall -std=gnu99
DEPS=-lrt -pthread

all:
	rm -f ctr_encryption
	$(CC) $(CFLAGS) ctr_encryption.c utils.c endecryption_direct_access.c -o ctr_encryption
