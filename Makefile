CC=gcc
CFLAGS=-g -Wall
LDFLAGS=
BINS=proxy

.PHONY: all clean

all: $(BINS)

%: %.o
	$(CC) $(LDFLAGS) $< -o $@

%.o: %.c
	$(CC) $< -c $(CFLAGS) -o $@

clean:
	rm -f *.o $(BINS)