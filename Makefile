
CFLAGS = -O3 -Wall -pedantic

all: v65 bootstrap

v65: 6502.o platform.o
	cc -o $@ $^

6502.c: 6502.h config.h

platform.c: 6502.h config.h

bootstrap: bootstrap.s bootstrap.ld
	cl65 -t none bootstrap.s -C bootstrap.ld
	dd if=bootstrap of=disk0 conv=notrunc

clean:
	rm -f bootstrap *.o *~ v65