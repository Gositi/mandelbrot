CC = gcc
OBJECTS = libbmp.c mandelbrot.c
LIBS =
CFLAGS = -Wall -Wextra -O3
BINDIR = $(DESTDIR)/usr/bin
NAME = mandelbrot

mandelbrot: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJECTS) $(LIBS) -lm -lpthread

clean:
	rm $(NAME)
