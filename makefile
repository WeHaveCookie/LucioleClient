EXECS= client

all: $(EXECS)

client: ./src/client.c ./src/game.c
	gcc -o client ./src/client.c ./src/game.c -lSDLmain -lSDL

clean:
	rm *.o
	rm client

.PHONY: clean

