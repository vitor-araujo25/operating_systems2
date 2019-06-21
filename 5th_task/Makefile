all: daemon summoner 
	./summoner 3
	./daemon 2 | awk '{print $$2}' > daemon.pid


daemon: daemon.c
	gcc daemon.c -o daemon

summoner: summoner.c
	gcc summoner.c -o summoner

clean:
	kill -TERM $$(cat daemon.pid)
	rm daemon summoner daemon.pid
	./slayer.sh
	

