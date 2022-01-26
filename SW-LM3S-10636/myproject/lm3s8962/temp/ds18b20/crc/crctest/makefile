all:
	gcc -c -o crc.o crc.c
	gcc -c -o hostcrc.o hostcrc.c
	gcc -o hostcrc hostcrc.o crc.o
clean:
	rm -fr crc.o hostcrc.o hostcrc
