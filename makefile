all: zlib.bc
	emcc --bind -s ALLOW_MEMORY_GROWTH=1 -O3 zlib.bc -o fgrc.js main.cpp

zlib.bc: zlib.h
	emcc -O3 zlib/adler32.c zlib/compress.c zlib/crc32.c zlib/deflate.c zlib/gzclose.c zlib/infback.c zlib/inffast.c zlib/inflate.c zlib/inftrees.c zlib/trees.c zlib/uncompr.c zlib/zutil.c -o zlib.bc