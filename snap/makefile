.PHONY: all install uninstall clean

name = tuxfishing

all:
	gcc ../main.c ../glad_gl.c -I ../inc -Ofast -lglfw -lm -o $(name)

install:
	cp $(name) $(DESTDIR)

uninstall:
	rm $(DESTDIR)/$(name)

clean:
	rm $(name)