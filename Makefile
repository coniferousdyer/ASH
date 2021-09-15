CC=gcc
IDIR = src/header_files
CFLAGS=-I$(IDIR)
FILES = src/*.c src/commands/*.c

DEPS = commands.h utilities.h util_variables.h

shell: $(FILES)
	$(CC) $^ -o $@ $(CFLAGS)