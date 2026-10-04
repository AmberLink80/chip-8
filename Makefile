CC      := gcc
BUILD   ?= release
SRC     := main.c chip-8.c
OBJDIR  := build/$(BUILD)
OBJ     := $(SRC:%.c=$(OBJDIR)/%.o)
BIN     := $(OBJDIR)/chip-8

CFLAGS  := -I. -Wall -Wextra -MMD -MP
LDLIBS  := -lSDL2

ifeq ($(BUILD),debug)
  CFLAGS += -g -O0 -DDEBUG
else
  CFLAGS += -O2 -DNDEBUG
endif

.PHONY: all debug clean

all: $(BIN)

debug:
	$(MAKE) BUILD=debug

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(LDLIBS)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf build
	rm -rf *.o

-include $(OBJ:.o=.d)
