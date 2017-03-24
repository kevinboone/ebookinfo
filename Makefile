VERSION := 0.0.1
CC      := gcc
CFLAGS  := -fPIC -Wall -DVERSION=\"$(VERSION)\" -g -I include
LIBS    := -lpcre 
INCLUDE := /usr/include/ebookinfo
DESTDIR := /usr
LIB     := libebookinfo.a
LIBDIR  := $(DESTDIR)/lib64/
TARGET	:= ebookinfo 
SO      := libebookinfo.so.$(VERSION)
SOURCES := $(shell find src/ -type f -name *.c)
OBJECTS := $(patsubst src/%,build/%,$(SOURCES:.c=.o))
UTIL_OBJS := build/main.o 
LIB_OBJS := build/ebook.o build/epub.o build/mobi.o build/rtf.o build/ebookmetadata.o build/sxmlc.o build/sxmlutils.o build/ebistring.o
DEPS	:= $(OBJECTS:.o=.deps)
MANDIR  := $(DESTDIR)/share/man

all: $(TARGET) $(SO)

$(TARGET): $(UTIL_OBJS) $(LIB)
	$(CC) -o $(TARGET) $(UTIL_OBJS) $(LIBS) $(LIB)

$(LIB): $(LIB_OBJS)
	ar r $(LIB) $(LIB_OBJS) 

$(SO): $(LIB)
	gcc -s -shared -o $(SO) $(LIB_OBJS) 

build/%.o: src/%.c
	@mkdir -p build/
	$(CC) $(CFLAGS) -MD -MF $(@:.o=.deps) -c -o $@ $<

clean:
	$(RM) -r build/ $(TARGET) $(LIB) $(SO)

install: $(TARGET)
	cp -p $(TARGET) ${DESTDIR}/bin/
	cp -vaux include/ebookinfo /usr/include
	cp -p $(SO) $(LIBDIR)
	(cd ${LIBDIR}; ln -sf ${SO} libebookinfo.so)
	cp -p man1/* ${MANDIR}/man1/

-include $(DEPS)

.PHONY: clean

