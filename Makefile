CSC      ?= csc
R2M      ?= rst2man.py
CSCFLAGS += -static

NAME = color
BIN  = bin/$(NAME)
MAN  = bin/$(NAME).1.gz

PREFIX ?= $(DESTDIR)/usr
BINDIR  = $(PREFIX)/bin
MANDIR  = $(PREFIX)/share/man/man1

all : CSCFLAGS += -O3
all : $(BIN) $(MAN)

debug : CSCFLAGS += -O0 -d3
debug : $(BIN) $(MAN)

bin/% : src/%.scm
	@mkdir -p bin
	CSC='$(CSC)' CSCFLAGS='$(CSCFLAGS)' ./csc.sh $< $@
	@rm -f bin/*.link

bin/%.1.gz : doc/%.rst
	$(R2M) $< | gzip -9 > $@

clean :
	rm -rf bin eggs

install : all
	install -Dm755 $(BIN) -t $(BINDIR)
	install -Dm644 $(MAN) -t $(MANDIR)

uninstall :
	rm -f $(BINDIR)/$(notdir $(BIN))
	rm -f $(MANDIR)/$(notdir $(MAN))

.PHONY: all debug clean install uninstall
