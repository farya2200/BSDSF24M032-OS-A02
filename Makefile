CC = gcc
CFLAGS = -Wall -Wextra -std=c11

TARGET = bin/ls
SOURCE = src/ls-v1.0.0.c
MANPAGE = man/ls.1

BINDIR = /usr/local/bin
MANDIR = /usr/local/share/man/man1

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) -o $(TARGET)

install: all
	install -d $(DESTDIR)$(BINDIR)
	install -d $(DESTDIR)$(MANDIR)
	install -m 755 $(TARGET) $(DESTDIR)$(BINDIR)/ls
	install -m 644 $(MANPAGE) $(DESTDIR)$(MANDIR)/ls.1

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/ls
	rm -f $(DESTDIR)$(MANDIR)/ls.1

clean:
	rm -f $(TARGET)

.PHONY: all install uninstall clean
