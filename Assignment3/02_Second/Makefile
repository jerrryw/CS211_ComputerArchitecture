CC        := gcc
NAME      := second
tests     := 1 2 3 4 6 7 9 10
SOURCE    := $(NAME).c
TARGET    := $(NAME)
ASAN_FLAG := -fsanitize=address
DIFF      := diff

all: release

.PHONY: release debug test mytest clean

release: 
	$(CC) $(SOURCE) -o $(TARGET) $(ASAN_FLAG) -Wall -Werror -std=c11 -O3

debug: 
	$(CC) $(SOURCE) -o $(TARGET) $(ASAN_FLAG) -Wall -Werror -std=c11 -g -DDEBUG

test: $(TARGET)
	@for i in $(tests); do \
		./$(TARGET) ../../testcases/$(NAME)/test$${i}.txt > result$${i}.txt ;\
		$(DIFF) result$${i}.txt ../../testcases/$(NAME)/result$${i}.txt && echo "\e[32mPass $${i} \e[0m" || echo "\e[31mFail $${i} \e[0m" >&2 ;\
	done

mytest: 
	./$(TARGET) mytest.txt > myresult.txt

clean:
	rm -f $(TARGET)
	rm -f result*.txt
	rm -f mytest.txt
	rm -f myresult.txt
