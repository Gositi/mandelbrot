CC = gcc
OBJECTS = mandelbrot.c
LIBS = libbmp.c
CFLAGS = -Wall -Wextra
BINDIR = $(DESTDIR)/usr/bin
NAME = mandelbrot

mandelbrot: $(OBJECTS)
	$(CC) $(CFLAGS) -O3 -o $(NAME) $(OBJECTS) $(LIBS) -lm -lpthread

debug: $(OBJECTS)
	$(CC) $(CFLAGS) -g -o $(NAME) $(OBJECTS) $(LIBS) -lm -lpthread

clean:
	rm $(NAME)
