#!/usr/bin/env python3
"""Convert a raw disk image (.wic) to an Android sparse image (.simg).

Zero-filled blocks become DONT_CARE chunks, so a fastboot flash transfers
and writes only the blocks that hold data; the rest of the eMMC keeps
whatever it had (a free ext4/vfat block has no meaning until it is
allocated; "fastboot erase mmc0" before the flash zeroes the device in
seconds if a clean part is wanted). U-Boot's fastboot writes such an
image to the whole user area with "fastboot flash mmc0 <file>".

No dependencies: the format is a 28-byte header followed by chunks of
RAW / FILL / DONT_CARE blocks, the same thing img2simg produces minus the
trailing CRC chunk, which U-Boot ignores anyway.

    wic2simg.py <image.wic> <image.simg> [--block-size 4096] [--fill-zero]
    wic2simg.py --verify <image.wic> <image.simg>

--fill-zero emits FILL(0) chunks instead of DONT_CARE: nothing more is
transferred, but U-Boot then writes the zeros, so the result is
byte-identical to a raw flash at the cost of eMMC write time.
"""
import argparse
import os
import struct
import sys

MAGIC = 0xED26FF3A
CHUNK_RAW, CHUNK_FILL, CHUNK_DONT_CARE, CHUNK_CRC = 0xCAC1, 0xCAC2, 0xCAC3, 0xCAC4
FILE_HDR = struct.Struct("<IHHHHIIII")   # magic, major, minor, file_hdr_sz, chunk_hdr_sz, blk_sz, total_blks, total_chunks, checksum
CHUNK_HDR = struct.Struct("<HHII")        # type, reserved, chunk_sz (blocks), total_sz (bytes, header included)
MAX_RAW_CHUNK = 64 << 20                  # keep raw chunks cuttable for the host's max-download-size resparse


def convert(src_path, dst_path, bs, fill_zero):
    size = os.path.getsize(src_path)
    total_blks = (size + bs - 1) // bs
    zero = b"\0" * bs
    max_raw_blocks = MAX_RAW_CHUNK // bs
    chunks = 0
    with open(src_path, "rb") as src, open(dst_path, "wb") as dst:
        dst.write(FILE_HDR.pack(MAGIC, 1, 0, FILE_HDR.size, CHUNK_HDR.size, bs, total_blks, 0, 0))
        run_type, run_blocks, run_data = None, 0, []

        def flush():
            nonlocal chunks, run_type, run_blocks, run_data
            if run_blocks == 0:
                return
            if run_type == CHUNK_RAW:
                dst.write(CHUNK_HDR.pack(CHUNK_RAW, 0, run_blocks, CHUNK_HDR.size + run_blocks * bs))
                dst.write(b"".join(run_data))
            elif run_type == CHUNK_FILL:
                dst.write(CHUNK_HDR.pack(CHUNK_FILL, 0, run_blocks, CHUNK_HDR.size + 4))
                dst.write(b"\0\0\0\0")
            else:
                dst.write(CHUNK_HDR.pack(CHUNK_DONT_CARE, 0, run_blocks, CHUNK_HDR.size))
            chunks += 1
            run_type, run_blocks, run_data = None, 0, []

        while True:
            d = src.read(bs)
            if not d:
                break
            if len(d) < bs:
                d = d + b"\0" * (bs - len(d))
            t = CHUNK_RAW if d != zero else (CHUNK_FILL if fill_zero else CHUNK_DONT_CARE)
            if t != run_type or (t == CHUNK_RAW and run_blocks >= max_raw_blocks):
                flush()
                run_type = t
            run_blocks += 1
            if t == CHUNK_RAW:
                run_data.append(d)
        flush()
        dst.seek(0)
        dst.write(FILE_HDR.pack(MAGIC, 1, 0, FILE_HDR.size, CHUNK_HDR.size, bs, total_blks, chunks, 0))
    out = os.path.getsize(dst_path)
    print(f"{src_path}: {size} bytes, {total_blks} blocks of {bs} -> {dst_path}: {out} bytes in {chunks} chunks "
          f"({100.0 * out / size:.0f}% of the raw image)")


def verify(src_path, simg_path):
    """Expand the sparse image in memory, block run by block run, against the raw image."""
    size = os.path.getsize(src_path)
    with open(src_path, "rb") as raw, open(simg_path, "rb") as simg:
        magic, major, minor, fhs, chs, bs, total_blks, total_chunks, _ = FILE_HDR.unpack(simg.read(FILE_HDR.size))
        assert magic == MAGIC and major == 1 and fhs == FILE_HDR.size and chs == CHUNK_HDR.size, "bad header"
        assert total_blks * bs >= size > (total_blks - 1) * bs, "block count does not cover the raw image"
        pos = 0
        zero = b"\0" * bs
        for _ in range(total_chunks):
            t, _, n, tot = CHUNK_HDR.unpack(simg.read(CHUNK_HDR.size))
            want = raw.read(n * bs)
            want = want + b"\0" * (n * bs - len(want))
            if t == CHUNK_RAW:
                assert tot == CHUNK_HDR.size + n * bs
                assert simg.read(n * bs) == want, f"raw mismatch at block {pos}"
            elif t == CHUNK_FILL:
                assert tot == CHUNK_HDR.size + 4 and simg.read(4) == b"\0\0\0\0"
                assert want == zero * n, f"fill mismatch at block {pos}"
            elif t == CHUNK_DONT_CARE:
                assert tot == CHUNK_HDR.size
                assert want == zero * n, f"dont-care covers data at block {pos}"
            else:
                raise AssertionError(f"unexpected chunk type {t:#x}")
            pos += n
        assert pos == total_blks, "chunk blocks do not add up to total_blks"
        assert simg.read(1) == b"", "trailing bytes after the last chunk"
    print(f"{simg_path}: verified against {src_path} ({total_chunks} chunks, {total_blks} blocks)")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("src")
    ap.add_argument("dst")
    ap.add_argument("--block-size", type=int, default=4096)
    ap.add_argument("--fill-zero", action="store_true")
    ap.add_argument("--verify", action="store_true", help="check an existing .simg against the raw image instead")
    a = ap.parse_args()
    if a.verify:
        verify(a.src, a.dst)
    else:
        convert(a.src, a.dst, a.block_size, a.fill_zero)


if __name__ == "__main__":
    sys.exit(main())
