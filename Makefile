ztorrent : main.o bencode.o torrent.o fgmp/gmp.o
	cc -o ztorrent torrent.o main.o bencode.o fgmp/gmp.o -lrhash -lcurl

main.o : main.c
	cc -c -g main.c

bencode.o : bencode.h bencode.c
	cc -c -g bencode.c

torrent.o : torrent.h torrent.c
	cc -c -g torrent.c

fgmp/gmp.o : fgmp/gmp.c fgmp/gmp.h
	cd fgmp; \
	make; \
	cd ..

clean : 
	rm ztorrent main.o bencode.o torrent.o fgmp/gmp.o
