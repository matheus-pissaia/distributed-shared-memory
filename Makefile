# Compiler and flags
CC       := mpicc
CFLAGS   := -Wall -Wextra

# Folders
SRCDIR   := src
OBJDIR   := obj
BINDIR   := bin

# Source & object files (recursive)
SRCS     := $(shell find $(SRCDIR) -type f -name '*.c')
OBJS     := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

# Final binary
TARGET   := $(BINDIR)/a.out

.PHONY: all prepare clean rebuild

all: prepare $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Compile each .c → .o, creating obj subdirs as needed
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Make sure base folders exist
prepare:
	mkdir -p $(OBJDIR) $(BINDIR)

clean:
	rm -rf $(OBJDIR) $(BINDIR)

rebuild: clean all
