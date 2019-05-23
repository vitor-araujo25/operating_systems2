output = saida
input = ex1.c isopen.c

all: $(output)
	./$(output)

$(output): $(input)
	gcc $(input) -o $(output)

clean:
	rm $(output)
