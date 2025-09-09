CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
TARGET = s21_grep
SOURCES = s21_grep.c
HEADERS = s21_grep.h

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)

rebuild: clean $(TARGET)

.PHONY: clean rebuild