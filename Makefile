CC     ?= gcc
R2M    ?= rst2man.py
CFLAGS += -pedantic -Wall -Wextra

NAME = color
BIN  = bin/$(NAME)
MAN  = doc/$(NAME).1.gz

PREFIX ?= $(DESTDIR)/usr
BINDIR  = $(PREFIX)/bin
MANDIR  = $(PREFIX)/share/man/man1

all : $(BIN) $(MAN)

bin/% : src/%.c
	@mkdir -p bin
	$(CC) $(CFLAGS) $< -o $@

doc/%.1.gz : doc/%.rst
	$(R2M) $< | gzip -9 > $@

clean :
	rm -rf bin
	rm -f doc/*.1.gz

install : all
	install -Dm755 $(BIN) -t $(BINDIR)
	install -Dm644 $(MAN) -t $(MANDIR)

uninstall :
	rm -f $(BINDIR)/$(NAME)
	rm -f $(MANDIR)/$(NAME).1.gz

.PHONY : all clean install uninstall
