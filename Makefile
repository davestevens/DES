CC = gcc
CFLAGS = -c -pedantic -Wall -Wextra -m32
LDFLAGS = -m32

SRC = $(wildcard *.c)
OBJ = $(SRC:%.c=%.o)

TARGET = DES

all: $(SRC) $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

distclean:
	rm -f $(OBJ) $(TARGET) *~ \#*