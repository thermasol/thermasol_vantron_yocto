#!/usr/bin/env python3
"""Assemble the eMMC boot0 image for vt-sbc-am62l.

The ROM reads the R5 SPL from offset 0 of the eMMC boot0 hardware
partition, the R5 SPL loads the A53 SPL (tispl.bin) from 0x80000 and the
A53 SPL loads U-Boot proper (u-boot.img) from 0x280000 - the three offsets
the DFU alts "tiboot3 raw 0x0 0x23d", "tispl raw 0x400 0x909" and "u-boot
raw 0x1400 0x1400" wrote to one at a time (sector units). One file at
those offsets lets a single "fastboot flash mmc0boot0 boot0.img" do it.

    make-boot0.py <tiboot3.bin> <tispl.bin> <u-boot.img> <boot0.img>
"""
import os
import sys

LAYOUT = (  # (name, offset, max size) in bytes, from the DFU alt table
    ("tiboot3.bin", 0x0,      0x23d * 512),
    ("tispl.bin",   0x80000,  0x909 * 512),
    ("u-boot.img",  0x280000, 0x1400 * 512),
)


def main(argv):
    if len(argv) != 5:
        sys.exit(__doc__)
    out = bytearray()
    for (name, off, limit), path in zip(LAYOUT, argv[1:4]):
        data = open(path, "rb").read()
        if len(data) > limit:
            sys.exit(f"{path}: {len(data)} bytes does not fit the {limit}-byte {name} slot")
        if len(out) > off:
            sys.exit(f"{name}: previous image runs past offset {off:#x}")
        out += b"\0" * (off - len(out))
        out += data
        print(f"{name:<12} {len(data):>8} bytes at {off:#09x} from {path}")
    with open(argv[4], "wb") as f:
        f.write(out)
    print(f"{argv[4]}: {len(out)} bytes")


if __name__ == "__main__":
    main(sys.argv)
