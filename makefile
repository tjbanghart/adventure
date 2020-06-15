CC = gcc
CFLAGS = -Wall -pedantic

adventure: adventure.o
	$(CC) $(CFLAGS) -o adventure adventure.o

buildrooms: buildrooms.o
	$(CC) $(CFLAGS) -o buildrooms buildrooms.o

buildrooms.o: buildrooms.c buildrooms.h
	$(CC) $(CFLAGS) -c buildrooms.c 

adventure.o: adventure.c adventure.h
	$(CC) $(CFLAGS) -c adventure.c

clean:
	rm -f -r banghart.* adventure.o buildrooms.o adventure buildrooms