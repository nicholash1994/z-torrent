ztorrent : main.o bencode.o http.o
	cc -o ztorrent main.o bencode.o http.o -lrhash

dbg : main.dbg.o bencode.dbg.o http.dbg.o
	cc -o ztorrent-dbg -g main.o bencode.o http.o -lrhash

main.o : main.c bencode.c bencode.h http.c http.h
	cc -c main.c

main.dbg.o : main.c bencode.c bencode.h http.c http.h
	cc -c -g main.c

bencode.o : bencode.h
	cc -c bencode.c

bencode.dbg.o : bencode.h
	cc -c -g bencode.c

http.o : http.c http.h bencode.c bencode.h /usr/include/rhash.h
	cc -c http.c

http.dbg.o : http.c http.h bencode.c bencode.h /usr/include/rhash.h
	cc -c -g http.c

clean : 
	rm ztorrent ztorrent-dbg main.o bencode.o http.o
