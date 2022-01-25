stars: main.c
	gcc -o stars main.c -lSDL2 -lm

run: stars
	./stars