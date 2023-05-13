
CCFLAGS = -Wall -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion -Wdouble-promotion -g3 -O2
CC = gcc

all:	t2s

t2s:	t2s.c
	$(CC) $(CCFLAGS) $< -o $@ $(LIBS) -lm

ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

install: t2s
	install -m 755 t2s     $(DESTDIR)$(PREFIX)/bin

clean:
	rm -f t2s t2s.o
