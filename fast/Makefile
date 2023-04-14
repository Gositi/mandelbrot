CC = gcc
OBJECTS = libbmp.c optibrot.c
LIBS =
CFLAGS = -Wall -Wextra -O3
BINDIR = $(DESTDIR)/usr/bin
NAME = optibrot

optibrot: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJECTS) $(LIBS) -lm

clean:
	rm $(NAME)
