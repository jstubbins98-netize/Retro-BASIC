CC      ?= cc
CFLAGS  = -Wall -Wextra -O2 -std=c99
TARGET  = basic
BUILD   = build

SRCS    = main.c io.c vars.c expr.c program.c commands.c repl.c

OBJS    = $(patsubst %.c,$(BUILD)/%.o,$(SRCS))

all: $(BUILD)/$(TARGET)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.o: %.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BUILD)/$(TARGET)

clean:
	rm -rf $(BUILD)

.PHONY: all clean
