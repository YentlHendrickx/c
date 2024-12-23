CC := clang
CFLAGS := -Wall -Wextra -Werror -Wpedantic -std=c11 -g

TARGET := linked_list
SRC := linked_list.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o ${TARGET} ${SRC}

clean:
	rm -f $(TARGET)
