# Virtual 65

This is a simple emulator designed to help bring up Fuzix on the BBC
Matchbox co-processor with banked RAM. At this point we don't try and
emulate the wait states on the external RAM but just run at about 4MHz.

The 6502 emulator is from MoarNES by Mike Chambers.

### License

GNU GPL v2.

### Memory Management

8K banked memory with bank registers selecting 8K pages from a 1MB RAM
range (128 banks) using bank identifiers 0-7. Memory between FE00-and FFEF
is mapped to I/O space always while FFF0-FFFF map to the RAM.

On boot the bank registers are 0,1,2,3,4,5,6,7.

### I/O Address Space (FE00-FEFF)

I/O space is 0xFE00 plus

FE00-FE07	r/w	bank number

FE10		ro	timer int clear (reports number of interrupts since
			last)

FE20		r	next input byte
FE20		w	output byte
FE21		r	input status (bit 0 - input pending bit 1 - write ready)

FE30		r/w	disk number
FE31		r/w	block high
FE32		r/w	block low	(512 byte blocks)
FE33		w	trigger disk action (sets diskstat, uses disk/block)
FE34		r/w	read or write next byte
FE35		r/w	disk status

FE40		w	Write 0xA5 to halt system


### Bootstrap

The imaginary boot ROM loads block 0 from disk to $0200 and jumps to it.

### Notes

Currently one large disc is implemented. It's meant to provide a simple
API akin to that we'll have calling through the tube firmware.

