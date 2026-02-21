CC = gcc

GTK3_CFLAGS = $(shell pkg-config --cflags gtk+-3.0)
GTK3_LIBS = $(shell pkg-config --libs gtk+-3.0)

GTK4_CFLAGS = $(shell pkg-config --cflags gtk4 2>/dev/null)
GTK4_LIBS = $(shell pkg-config --libs gtk4 2>/dev/null)

TARGETS = combobox-touch-test-gtk3 \
          dropdown-touch-test-gtk4 \
          gtk3-demo-combobox-original \
          gtk3-demo-combobox-fixed

all: $(TARGETS)

combobox-touch-test-gtk3: main-gtk3.c
	$(CC) $(GTK3_CFLAGS) -o $@ $< $(GTK3_LIBS)

dropdown-touch-test-gtk4: main-gtk4.c
	$(CC) $(GTK4_CFLAGS) -o $@ $< $(GTK4_LIBS)

gtk3-demo-combobox-original: gtk3-demo-combobox-original.c
	$(CC) $(GTK3_CFLAGS) -o $@ $< $(GTK3_LIBS)

gtk3-demo-combobox-fixed: gtk3-demo-combobox-fixed.c
	$(CC) $(GTK3_CFLAGS) -o $@ $< $(GTK3_LIBS)

clean:
	rm -f $(TARGETS)

.PHONY: all clean
