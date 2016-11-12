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

| Address | R/W | Description
| ------- | --- | -----------
| FE00-7  | r/w	| Bank number
| FE10    | ro	| Timer int clear (reports interrupts since last)
| FE20    | r   | Next input byte
| FE20    | w   | Output byte
| FE21    | ro  | Input status (bit 0 - input pending bit 1 - write ready)
| FE30    | r/w | Disk number
| FE31    | r/w | Block high
| FE32    | r/w | Block low	(512 byte blocks)
| FE33    | w   | Trigger disk action (sets diskstat, uses disk/block)
| FE34    | r/w | Read or write next byte
| FE35    | ro  | Disk status (clear on read)
| FE40    | w   | Write 0xA5 to halt system

### Bootstrap

The imaginary boot ROM loads block 0 from disk to $0200 and jumps to it.

### Notes

Currently one large disc is implemented. It's meant to provide a simple
API akin to that we'll have calling through the tube firmware.
