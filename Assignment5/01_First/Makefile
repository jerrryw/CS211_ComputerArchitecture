all:
	gcc first.c -Wall -Werror -fsanitize=address -o first

clean:
	rm -rf first

run: first
	./first 512 direct fifo 16 "/mnt/c/Users/user/Desktop/Computer Science/Computer Architecture/Assignment 5/pa5_autograder/testcases/first/input/trace1.txt" > one_1.txt
	diff one_1.txt ../../testcases/first/output/one_1.txt
