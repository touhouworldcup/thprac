#!/usr/bin/env python3
from argparse import ArgumentParser
import lzma
import os

LZMA_LC = 4
LZMA_LP = 0
LZMA_PB = 1
LZMA_DICSIZE = 0x800000

def main():
    ap = ArgumentParser()
    ap.add_argument("exe")
    ap.add_argument("dll32")
    ap.add_argument("dll64")
    ap.add_argument("-o", "--output", default="payload.bin",
                    help="LZMA stream filename (default: payload.bin)")

    ap.add_argument("--header", default="thprac_sfx_lzma_config.h",
                    help="C header filename (default: payload.h)")

    a = ap.parse_args()

    files = [open(p, "rb").read() for p in (a.exe, a.dll32, a.dll64)]
    sizes = [len(f) for f in files]
    data = b"".join(files)

    payload = lzma.compress(data, format=lzma.FORMAT_RAW, filters=[
        {
            "id": lzma.FILTER_LZMA1,
            "preset": 9 | lzma.PRESET_EXTREME,
            "dict_size": LZMA_DICSIZE,
            "lc": LZMA_LC,
            "lp": LZMA_LP,
            "pb": LZMA_PB,
            "nice_len": 273,
            "mf": lzma.MF_BT4
        }
    ])

    with open(a.output, "wb") as f:
        f.write(payload)

    header = a.header or os.path.join(os.path.dirname(a.output), "payload.h")
    with open(header, "w") as f:
        f.write(f"#define LZMA_LC      {LZMA_LC}\n"
                f"#define LZMA_LP      {LZMA_LP}\n"
                f"#define LZMA_PB      {LZMA_PB}\n"
                f"#define LZMA_DICSIZE {LZMA_DICSIZE}\n"
                f"#define EXE_SIZE     {sizes[0]}\n"
                f"#define DLL32_SIZE   {sizes[1]}\n"
                f"#define DLL64_SIZE   {sizes[2]}\n")


if __name__ == "__main__":
    main()
