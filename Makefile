ztorrent : main.o bencode.o torrent.o torrent.h
	cc -o ztorrent torrent.o main.o bencode.o -lrhash -lcurl

main.o : main.c
	cc -c -g main.c -lcurl

bencode.o : bencode.h bencode.c
	cc -c -g bencode.c -lcurl

torrent.o : torrent.h torrent.c
	cc -c torrent.c -lcurl

clean : 
	rm ztorrent main.o bencode.o
