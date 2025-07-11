# Compiler and flags
CC = mpicc
CFLAGS = -Wall -Wextra

# Folders
SRCDIR = src
OBJDIR = obj
BINDIR = bin

# Files
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))
TARGET = $(BINDIR)/a.out

# Main rules
all: prepare $(TARGET)

# Creating the final binary
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Compiling each .c file
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Preparing each target folder
prepare:
	mkdir -p $(OBJDIR) $(BINDIR)

# Cleaning the project
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Recompile from scratch
rebuild: clean all
