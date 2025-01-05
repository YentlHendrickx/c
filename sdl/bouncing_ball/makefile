CC := clang
CFLAGS := -Wall -Wextra -Werror -Wpedantic -std=c11 -g $(shell sdl2-config --cflags)
LDFLAGS := $(shell sdl2-config --libs) -lm

TARGET := bouncing_ball.out
SRC := bouncing_ball.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)
