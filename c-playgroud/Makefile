CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -O2
LDFLAGS = -L../ic-agent-wrapper/target/release -lic_agent_wrapper -lSystem -lm -lc

SRCDIR = .
BINDIR = .

SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(SRC:.c=.o)
LIBS = $(wildcard $(LIBDIR)/*.a)

TARGET = $(BINDIR)/test
DEPS = test.h
all: $(TARGET)

$(TARGET): $(OBJ) $(LIBS)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

$(OBJ): %.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
