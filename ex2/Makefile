output = saida
input = walk_dir.c

all: $(output)
	./$(output)

$(output): $(input)
	gcc $(input) -o $(output)

clean:
	rm $(output)
