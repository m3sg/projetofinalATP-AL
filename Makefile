CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2
LDFLAGS = -lm
TARGET = projetofinalATPAL
SRCS = main.c matriz.c parser.c sistema.c transformacao.c autovaloresAutovetores.c menu.c

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: clean
