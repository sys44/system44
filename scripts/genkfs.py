#!/usr/bin/env python3
import os, struct
KFS_MAGIC = 0x3053464B
KFS_MAX_FILES = 8
KFS_SECTOR_SIZE = 512
ROOTFS_DIR = "../rootfs"
OUTPUT_IMG = "kfs.img"
files = [f for f in os.listdir(ROOTFS_DIR) if os.path.isfile(os.path.join(ROOTFS_DIR, f))][:KFS_MAX_FILES]
superblock = struct.pack("<II", KFS_MAGIC, len(files))
file_entries = b""
current_lba = 1
file_data = b""

for f in files:
    path = os.path.join(ROOTFS_DIR, f)
    data = open(path,"rb").read()
    size = len(data)
    sectors = (size + KFS_SECTOR_SIZE - 1) // KFS_SECTOR_SIZE
    name_bytes = f.encode("ascii")[:32].ljust(32,b'\x00')
    file_entries += struct.pack("<32sII", name_bytes, current_lba, size)
    current_lba += sectors
    file_data += data + b'\x00' * (sectors*KFS_SECTOR_SIZE - size)

file_entries += b'\x00' * (40 * (KFS_MAX_FILES - len(files)))
superblock += file_entries
superblock += b'\x00' * (KFS_SECTOR_SIZE - len(superblock))

with open(OUTPUT_IMG,"wb") as img:
    img.write(superblock)
    img.write(file_data)