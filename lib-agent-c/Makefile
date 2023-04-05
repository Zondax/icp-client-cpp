CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -O2
LDFLAGS = -L../ic-agent-wrapper/target/release -lic_agent_wrapper -lSystem -lm -lc -framework Security  -framework CoreFoundation

SRCDIR = .
BINDIR = .
INCDIR = lib
SRC = $(wildcard $(SRCDIR)/*.c)
SRC += $(wildcard $(INCDIR)/*.c)
OBJ = $(SRC:.c=.o)
LIBS = $(wildcard $(LIBDIR)/*.a)

TARGET = $(BINDIR)/test

$(TARGET): $(OBJ) $(LIBS)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

$(OBJ): %.o: %.c
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
