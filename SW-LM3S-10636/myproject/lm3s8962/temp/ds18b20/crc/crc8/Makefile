all:
	gcc -c -o crc8.o crc8.c
	gcc -c -o main.o main.c
	gcc -o main main.o crc8.o
clean:
	rm -fr *.o main
