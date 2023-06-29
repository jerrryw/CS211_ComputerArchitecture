all: sixth
	
sixth: sixth.c
	gcc sixth.c -o sixth

test:
	gcc -Wall -Werror -fsanitize=address sixth.c -o sixth

clean: 
	rm -f sixth
