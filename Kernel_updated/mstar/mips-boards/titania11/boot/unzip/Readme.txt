Files:
    LzmaDecode.c/LzmaDecode.h: no modification from LZMA SDK Version 4.32
        LZMA decompression engine
    LzmaDec.c: created by Kevin
        API for decompression used by boot code or application
    lzma.exe: no modification from LZMA SDK Version 4.32

Compress:
    lzma.exe e venus.bin compressed.bin -lc0 -lp0

Decompress:
    Note: must allocate a memory block for internal structures. see LzmaDec.c for size requirement
    test on host: lzmaDec.exe compressed.bin decompressed.bin
    work on target: LzmaDec((const BYTE *)0xA1000000, (BYTE *)0xA4000000, (BYTE *)0xA8000000);

