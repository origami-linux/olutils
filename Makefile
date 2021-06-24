compiler = clang
ar = ar

cflags = -std=gnu99 -Ofast -Os -pedantic -Wall -Wextra -Iinclude

all: x86_64/libolutils.a

x86_64/olutils.o: olutils.c
	$(compiler) -c $< -target x86_64-elf-linux $(cflags) -o $@

x86_64/libolutils.a: x86_64/olutils.o
	$(ar) rcs $@ $<

clean:
	rm -f x86_64/libolutils.a x86_64/olutils.o
