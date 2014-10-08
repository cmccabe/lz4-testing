all: decompress1 compress1 compress-chunks cstream dstream

decompress1: decompress1.c lz4.c lz4.h
	gcc -Wall decompress1.c lz4.c lz4.h -o decompress1

compress1: compress1.c lz4.c lz4.h
	gcc -Wall compress1.c lz4.c lz4.h -o compress1

compress-chunks: compress-chunks.c lz4.c lz4.h
	gcc -Wall compress-chunks.c lz4.c lz4.h -o compress-chunks

cstream: cstream.c lz4.c lz4.h
	gcc -Wall cstream.c lz4.c lz4.h -o cstream

dstream: dstream.c lz4.c lz4.h
	gcc -Wall dstream.c lz4.c lz4.h -o dstream

clean:
	rm -f *.o decompress1 compress1 compress-chunks cstream dstream
