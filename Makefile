CC = x86_64-w64-mingw32-gcc
OBJECTS = mandelbrot.c
LIBS = libbmp.c
CFLAGS = -Wall -Wextra --static -O3
BINDIR = $(DESTDIR)/usr/bin
NAME = mandelbrot

mandelbrot: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJECTS) $(LIBS) -lm -lpthread
	
clean:
	rm $(NAME)
