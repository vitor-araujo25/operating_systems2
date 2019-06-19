all: parse summoner 
	./summoner 3
	./parse 1

parse: parse.c
	gcc parse.c -o parse

summoner: summoner.c
	gcc summoner.c -o summoner

clean:
	rm parse summoner daemon.log
	./slayer.sh

