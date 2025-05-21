CC := clang
CFLAGS := -Wall -Wextra -Werror -Wpedantic -std=c11 -g

TARGET := hash-map.out
SRC := hash-map.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o ${TARGET} ${SRC}

clean:
	rm -f $(TARGET)
