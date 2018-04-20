#
# Makefile for proxy
#

CC=gcc
CFLAGS=-g -Wall
LDFLAGS=
BINS=proxy

.PHONY: all clean

all: $(BINS)

proxy: proxy.o http_support.o
proxy.o: proxy.c http_support.h
http_support.o: http_support.c

$(BINS):
	$(CC) $^ -o $@

%.o:
	$(CC) $< -c $(CFLAGS) -o $@

clean:
	rm -f *.o $(BINS)

