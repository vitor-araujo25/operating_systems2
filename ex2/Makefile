all: saida
	./saida

saida: icount.o walk_dir.o
	gcc -g icount.o walk_dir.o -o saida

icount.o:
	gcc -g -c -o icount.o icount.c

walk_dir.o:
	gcc -g -c -o walk_dir.o walk_dir.c

clean:
	rm saida saidateste *.o

teste: saidateste
	./saidateste

saidateste: teste.o walk_dir.o
	gcc teste.o walk_dir.o -o saidateste

teste.o:
	gcc -c -o teste.o teste.c

