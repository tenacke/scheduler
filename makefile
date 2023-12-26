all: main.o hash.o heap.o reader.o types.h
	gcc -o scheduler main.o hash.o heap.o reader.o

main.o: main.c
	gcc -c main.c

hash.o: hash.c hash.h
	gcc -c hash.c

heap.o: heap.c heap.h
	gcc -c heap.c

reader.o: reader.c reader.h
	gcc -c reader.c

clean:
	rm -f *.o scheduler