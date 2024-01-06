all: main.o hash.o heap.o reader.o types.h
	gcc -o scheduler main.o hash.o heap.o reader.o

main.o: main.c
	gcc -O3 -c main.c

hash.o: hash.c hash.h
	gcc -O3 -c hash.c

heap.o: heap.c heap.h
	gcc -O3 -c heap.c

reader.o: reader.c reader.h
	gcc -O3 -c reader.c

clean:
	rm -f *.o scheduler