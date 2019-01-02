ztorrent : main.o bencode.o torrent.o torrent.h
	cc -o ztorrent torrent.o main.o bencode.o -lrhash -lcurl

main.o : main.c bencode.c bencode.h
	cc -c -g main.c -lcurl

bencode.o : bencode.h
	cc -c -g bencode.c -lcurl

torrent.o : torrent.h bencode.h bencode.c
	cc -c torrent.c -lcurl

clean : 
	rm ztorrent main.o bencode.o
