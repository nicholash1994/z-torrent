ztorrent : main.o bencode.o http.o
	cc -o ztorrent main.o bencode.o http.o -lrhash

main.o : main.c bencode.c bencode.h http.c http.h
	cc -c -g main.c

bencode.o : bencode.h
	cc -c -g bencode.c

torrent.o : torrent.h bencode.h bencode.c

clean : 
	rm ztorrent main.o bencode.o http.o
