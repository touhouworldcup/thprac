#!/usr/bin/env python3
"""Pack thprac's exe/DLLs into an LZMA payload and build the SFX stub.

All generated files (payload, config header, .res, .o, final thprac.exe)
are written to the "build" subfolder next to this script.
"""
from argparse import ArgumentParser
from pathlib import Path
import lzma
import shutil
import subprocess
import sys

LZMA_LC = 4
LZMA_LP = 0
LZMA_PB = 1
LZMA_DICSIZE = 0x800000

SCRIPT_DIR = Path(__file__).resolve().parent
C_SOURCE = SCRIPT_DIR / "thprac_sfx_lzma.c"
RC_SOURCE = SCRIPT_DIR.parent / "thprac" / "thprac_launcher.rc"
RELEASE_DIR = SCRIPT_DIR.parent / "Release"

PAYLOAD_NAME = "payload.bin"
HEADER_NAME = "thprac_sfx_lzma_config.h"
CERT_NAME  = "cert.pfx"
RES_NAME = "thprac_sfx.res"
OBJ_NAME = "thprac_sfx_lzma.o"
OUT_NAME = "thprac.exe"


def find_tool(name):
    path = shutil.which(name)
    if path is None:
        sys.exit(f"error: '{name}' was not found on PATH")
    return path


def run(cmd):
    print("> " + " ".join(str(c) for c in cmd), flush=True)
    try:
        subprocess.run([str(c) for c in cmd], check=True)
    except subprocess.CalledProcessError as e:
        sys.exit(f"error: command failed with exit code {e.returncode}")


def pack(inputs, build_dir):
    files = [Path(p).read_bytes() for p in inputs]
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

    (build_dir / PAYLOAD_NAME).write_bytes(payload)

    with open(build_dir / HEADER_NAME, "w", newline="\n") as f:
        f.write(f"#define LZMA_LC      {LZMA_LC}\n"
                f"#define LZMA_LP      {LZMA_LP}\n"
                f"#define LZMA_PB      {LZMA_PB}\n"
                f"#define LZMA_DICSIZE {LZMA_DICSIZE}\n"
                f"#define EXE_SIZE     {sizes[0]}\n"
                f"#define DLL32_SIZE   {sizes[1]}\n"
                f"#define DLL64_SIZE   {sizes[2]}\n")


def build(build_dir):
    llvm_rc = find_tool("llvm-rc")
    clang = find_tool("clang")
    lld_link = find_tool("lld-link")

    if not llvm_rc or not clang or not lld_link:
        print("A required tool was not found. Required tools are: llvm-rc, clang, lld-link. Make sure all of these tools are in your PATH")
        return False

    run([llvm_rc, "/FO", build_dir / RES_NAME, RC_SOURCE])

    run([
        clang, "-m32", "-std=c23", "-Os",
        "-ffreestanding", "-fno-stack-protector",
        "-fno-unwind-tables", "-fno-asynchronous-unwind-tables",
        "-c", C_SOURCE,
        "-o", build_dir / OBJ_NAME,
    ])

    run([
        lld_link,
        "/machine:x86", "/subsystem:windows", "/entry:start",
        "/nodefaultlib", "/fixed", "/merge:.rdata=.text",
        "/opt:ref", "/safeseh:no",
        f"/out:{build_dir / OUT_NAME}",
        build_dir / OBJ_NAME, build_dir / RES_NAME, "kernel32.lib",
    ])

    return True


def main():
    ap = ArgumentParser(description=__doc__)
    ap.add_argument("exe",   nargs='?', default=RELEASE_DIR / "thprac.exe")
    ap.add_argument("dll32", nargs='?', default=RELEASE_DIR / "thprac32.dll")
    ap.add_argument("dll64", nargs='?', default=RELEASE_DIR / "thprac64.dll")
    ap.add_argument("-b", "--build-dir", type=Path,
                    default=SCRIPT_DIR / "build",
                    help="output directory (default: ./build next to this script)")
    ap.add_argument("--cert", type=Path,
                    default=SCRIPT_DIR.parent / "cert.pfx",
                    help="certificate to sign with")
    a = ap.parse_args()

    build_dir = a.build_dir.resolve()
    build_dir.mkdir(parents=True, exist_ok=True)

    pack([a.exe, a.dll32, a.dll64], build_dir)

    if build(build_dir):
        print(f"\nDone: {build_dir / OUT_NAME}")

    if a.cert.exists():
        signtool = find_tool("signtool")
        run([signtool, "sign", "/fd", "certHash", "/v", "/f", a.cert, "/d", "thprac", "/du", "https://github.com/touhouworldcup/thprac", build_dir / OUT_NAME])
    else:
        print("Warning: No signature found at " + a.cert)


if __name__ == "__main__":
    main()