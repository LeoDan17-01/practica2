CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -g
TARGETS = emisor receptor

all: $(TARGETS)

emisor: src/emisor.c
	$(CC) $(CFLAGS) -o $@ $<

receptor: src/receptor.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean