# Decided to declare PPM as the base format - GJ06

FILES		= $(wildcard *.ppm)
IMAGES_U16	= $(FILES:.ppm=.u16)
IMAGES_PNG	= $(FILES:.ppm=.png)
IMAGES_RAW	= $(FILES:.ppm=.raw)
IMAGES_GEN	= $(IMAGES_U16)

.SUFFIXES: .raw .u16 .png .ppm

all: $(IMAGES_GEN)

# Old stuff work like this:
#volume.u16: volume.raw
#	../build/i386/conv $< $@ 300 62
#
#volume.ppm: volume.raw
#	convert -size 300x62 -depth 8 rgb:$< $@

%.raw: %.ppm
	tail -3 $< $@

%.u16: %.ppm
	../build/i386/ppm2u16 $< $@

%.png: %.ppm
	convert $< $@

clean:
	rm -f $(IMAGES_GEN)
