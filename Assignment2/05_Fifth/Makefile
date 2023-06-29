all: fifth
	
fifth: fifth.c
	gcc fifth.c -o fifth

test:
	gcc -Wall -Werror -fsanitize=address fifth.c -o fifth

clean: 
	rm -f fifth
