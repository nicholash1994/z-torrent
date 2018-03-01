ztorrent : main.o bencode.o http.o
	cc -o ztorrent main.o bencode.o http.o -lrhash

main.o : main.c bencode.c bencode.h http.c http.h
	cc -c -g main.c

bencode.o : bencode.h
	cc -c -g bencode.c

http.o : http.c http.h bencode.c bencode.h /usr/include/rhash.h
	cc -c -g http.c

clean : 
	rm ztorrent main.o bencode.o http.o
