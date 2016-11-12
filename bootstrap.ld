MEMORY {
	RAMZ:	start = $0000, size = $0100, type = rw, define = yes;
	RAM0:	start = $0200, size = $0100, type = rw, define = yes;
}

SEGMENTS {
	ZEROPAGE: load = RAMZ, type = zp, define = yes;
	BOOT: load = RAM0, type = ro;
}

FILES {
	%O: format = bin;
}
