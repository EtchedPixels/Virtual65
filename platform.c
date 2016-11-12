#include "config.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "6502.h"

static uint8_t ram[128][8192];

static uint8_t bank[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };

static uint8_t timer_int;
static uint16_t blk;
static uint8_t disk;
static uint8_t diskstat;

FILE *diskfile;

static uint8_t check_chario(void)
{
	fd_set i, o;
	struct timeval tv;
	unsigned int r = 0;

	FD_ZERO(&i);
	FD_SET(0, &i);
	FD_ZERO(&o);
	FD_SET(1, &o);
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	if (select(2, &i, NULL, NULL, &tv) == -1) {
		perror("select");
		exit(1);
	}
	if (FD_ISSET(0, &i))
		r |= 1;
	if (FD_ISSET(1, &o))
		r |= 2;
	return r;
}

static unsigned int next_char(void)
{
	char c;
	if (read(0, &c, 1) != 1) {
		printf("(tty read without ready byte)\n");
		return 0xFF;
	}
	return c;
}

static uint8_t io_read(uint16_t addr)
{
	addr -= 0xFE00;
	if (addr < 8)
		return bank[addr];
	if (addr == 0x10) {
		uint8_t v = timer_int;
		timer_int = 0;
		return v;
	}
	if (addr == 0x20)
		return next_char();
	if (addr == 0x21)
		return check_chario();
	if (addr == 0x30)
		return disk;
	if (addr == 0x31)
		return blk >> 8;
	if (addr == 0x32)
		return blk & 0xFF;
	if (addr == 0x34)
		return fgetc(diskfile);
	if (addr == 0x35) {
		uint8_t v = diskstat;
		diskstat = 0;
		return v;
	}
	return 0xFF;
}

static void io_write(uint16_t addr, uint8_t value)
{
	addr -= 0xFE00;
	if (addr < 8) {
		bank[addr] = value & 127;
		return;
	}
	if (addr == 0x20) {
		putchar(value);
		fflush(stdout);
		return;
	}
	if (addr == 0x30) {
		disk = value;
		return;
	}
	if (addr == 0x31) {
		blk &= 0xFF;
		blk |= value << 8;
		return;
	}
	if (addr == 0x32) {
		blk &= 0xFF00;
		blk |= value;
		return;
	}
	if (addr == 0x33) {
		if (disk != 0)
			diskstat = 0x02;
		else
			diskstat = (fseek(diskfile, blk << 9, SEEK_SET) == -1) ? 0x01 : 0;
		return;
	}
	if (addr == 0x34) {
		fputc(value, diskfile);
		return;
	}
	if (addr == 0x40 && value == 0xA5)
		exit(0);
}

uint8_t read6502(uint16_t addr)
{
	uint8_t bnum = bank[addr >> 13];
	if (addr >= 0xFE00 && addr <= 0xFFF0)
		return io_read(addr);
	else {
//        printf("R Bank %d Off %d\n", bnum, addr & 0x1FFF);
		return ram[bnum][addr & 0x1FFF];
	}
}

void write6502(uint16_t addr, uint8_t value)
{
	uint8_t bnum = bank[addr >> 13];
	if (addr >= 0xFE00 && addr <= 0xFFF0)
		io_write(addr, value);
	else {
//        printf("W Bank %d Off %d %02X\n", bnum, addr & 0x1FFF, value);
		ram[bnum][addr & 0x1FFF] = value;
	}
}

static void system_process(void)
{
	if (timer_int)
		irq6502();
}

int main(int argc, char *argv[])
{
	diskfile = fopen("disk0", "r+");
	if (diskfile == NULL) {
		perror("disk0");
		exit(1);
	}
	if (fread(ram[0] + 512, 512, 1, diskfile) != 1) {
		fprintf(stderr, "Unable to read boot image.\n");
		exit(1);
	}

	write6502(0xFFFC, 0x00);
	write6502(0xFFFD, 0x02);

	reset6502();

	hookexternal(system_process);
	while (1) {
		exec6502(41943);
		timer_int++;
	}
}
