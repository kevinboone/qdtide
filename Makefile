NAME    := qdtide
VERSION := 0.0.1
CC      := gcc
LIBS    := 
TARGET	:= $(NAME) 
SOURCES := $(shell find src/ -type f -name *.c)
OBJECTS := $(patsubst src/%,build/%,$(SOURCES:.c=.o))
DEPS	:= $(OBJECTS:.o=.deps)
DESTDIR := /usr
MANDIR  := $(DESTDIR)/share/man
BINDIR  := $(DESTDIR)/bin
SHARE   := /usr/share/$(TARGET)
CFLAGS  := -Wall -DNAME=\"$(NAME)\" -DVERSION=\"$(VERSION)\" -g -I include

all: $(TARGET)

$(TARGET): $(OBJECTS) 
	$(CC) -o $(TARGET) $(OBJECTS) $(LIBS) 

build/%.o: src/%.c
	@mkdir -p build/
	$(CC) $(CFLAGS) -MD -MF $(@:.o=.deps) -c -o $@ $<

clean:
	@echo "  Cleaning..."; $(RM) -r build/ $(TARGET)

install: $(TARGET)
	cp -p $(TARGET) ${BINDIR}
	cp -p man1/* ${MANDIR}/man1/

-include $(DEPS)

.PHONY: clean

