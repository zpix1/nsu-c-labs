all: debug
prod:
	$(CC) -O3 main.c notes.c -o nsbook -lncurses

debug:
	$(CC) -Wall -Wextra -g main.c notes.c -o nsbook -lncurses

test:
	$(CC) -Wall -g test.c notes.c -o nsbook_test -lncurses

clean:
	rm -r nsbook{,_test} *.dSYM a.out {,t}data.bin valgrind.txt