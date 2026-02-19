```make
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -I./src
TARGET = test_checkpoint
SRCDIR = src
TESTDIR = tests

_OBJ = checkpoint.o
OBJ = $(patsubst %,$(SRCDIR)/%,$(_OBJ))

_DEPS = checkpoint.h
DEPS = $(patsubst %,$(SRCDIR)/%,$(_DEPS))

$(SRCDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TESTDIR)/%.o: $(TESTDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ) $(TESTDIR)/test_checkpoint.o
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(SRCDIR)/*.o $(TESTDIR)/*.o $(TARGET)
