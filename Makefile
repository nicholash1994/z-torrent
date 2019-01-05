ztorrent : main.o bencode.o torrent.o
	cc -o ztorrent torrent.o main.o bencode.o -lrhash -lcurl

main.o : main.c
	cc -c -g main.c

bencode.o : bencode.h bencode.c
	cc -c -g bencode.c

torrent.o : torrent.h torrent.c
	cc -c -g torrent.c

clean : 
	rm ztorrent main.o bencode.o torrent.o
