CC=gcc
CFLAGS=-g -Wall
LDFLAGS=
BINS=client server client-simple server-simple

.PHONY: all clean

all: $(BINS)

%: %.o
	$(CC) $(LDFLAGS) $< -o $@

%.o: %.c
	$(CC) $< -c $(CFLAGS) -o $@

clean:
	rm -f *.o $(BINS)

