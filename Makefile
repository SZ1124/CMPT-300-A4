CC = gcc
CFLAGS = -std=c99 -Wall -g -D_POSIX_C_SOURCE=2000809L

SRC = UnixLs.c 
OBJ = $(SRC:.c=.o) list.o
HDR = list.h

TARGET = UnixLs

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(filter-out list.o, $(OBJ)) $(TARGET)
