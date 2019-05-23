all: icount
	./icount

icount: icount.o walk_dir.o
	gcc -g icount.o walk_dir.o -o icount

icount.o:
	gcc -g -c -o icount.o icount.c

walk_dir.o:
	gcc -g -c -o walk_dir.o walk_dir.c

clean:
	rm icount *.o


