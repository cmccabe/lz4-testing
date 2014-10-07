all: decompress1 compress1

decompress1: decompress1.c lz4.c lz4.h
	gcc -Wall decompress1.c lz4.c lz4.h -o decompress1

compress1: compress1.c lz4.c lz4.h
	gcc -Wall compress1.c lz4.c lz4.h -o compress1

clean:
	rm -f *.o decompress1 compress1
