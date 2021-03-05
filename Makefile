CSC      ?= csc
R2M      ?= rst2man.py
CSCFLAGS += -static -O3

NAME = color
BIN  = bin/$(NAME)
MAN  = bin/$(NAME).1.gz

PREFIX ?= $(DESTDIR)/usr
BINDIR  = $(PREFIX)/bin
MANDIR  = $(PREFIX)/share/man/man1

all : $(BIN) $(MAN)

bin/% : src/%.scm
	@mkdir -p bin
	CSC='$(CSC)' CSCFLAGS='$(CSCFLAGS)' ./make.sh $< $@
	@rm -f bin/*.link

bin/%.1.gz : doc/%.rst
	$(R2M) $< | gzip -9 > $@

clean :
	rm -rf bin eggs

install : all
	install -Dm755 $(BIN) -t $(BINDIR)
	install -Dm644 $(MAN) -t $(MANDIR)

uninstall :
	rm -f $(BINDIR)/$(basename $(BIN))
	rm -f $(MANDIR)/$(basename $(MAN))

.PHONY: all clean install uninstall
