.PHONY: sh tag debug

all: sh tag 

CC=gcc
CPP=g++
COBJS=$(patsubst  %.c,%.o,$(wildcard *.c))
CPPOBJS=$(patsubst %.cc,%.o,$(wildcard *.cc))
CFLAGS=-g  -O0
LDFLAGS=-lev

debug: $(COBJS) $(CPPOBJS)
	$(CPP) $(LDFALGS) -DDEBUG=1 -o sh $^ 

sh: $(COBJS) $(CPPOBJS)
	$(CPP)  $(LDFLAGS) -o $@ $^ 

%.o: %.c
	$(CC) $(CFLAGS) -std=c99 -c $<

%.o: %.cc
	$(CPP) $(CFLAGS) -c $<

tag:
	ctags -R *

clean:
	rm -rf *.o sh
