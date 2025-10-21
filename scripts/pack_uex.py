#!/usr/bin/env python3
import struct, sys, os
if len(sys.argv) != 4:
    print("usage: pack_uex.py <input.bin> <load_addr_hex> <out.uex>")
    sys.exit(2)
inp = sys.argv[1]
loadAddr = int(sys.argv[2], 16)
outf = sys.argv[3]
with open(inp, "rb") as f:
    data = f.read()
fileSize = len(data)
memSize = (fileSize + 0xFFF) & ~0xFFF
magic = 0x31555845
entry = loadAddr
hdr = struct.pack("<6I", magic, entry, loadAddr, fileSize, memSize, 0)
with open(outf, "wb") as f:
    f.write(hdr)
    f.write(data)
print(f"loadAddr=0x{loadAddr:x} fileSize={fileSize} memSize={memSize}")