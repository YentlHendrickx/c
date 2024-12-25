CC := clang
CFLAGS := -Wall -Wextra -Werror -Wpedantic -std=c11 -g

TARGET := linked_list_double.out
SRC := linked_list_double.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o ${TARGET} ${SRC}

clean:
	rm -f $(TARGET)
