.PHONY : all clean

CC := $(TOOL_CC)
AR := $(TOOL_AR)

MODULE_NAME := ft28

CFLAGS := -DFT2_BUILD_LIBRARY=1

C_INCLUDES := \
	include \
	src

SRC_FILES := \
	src/base/ftsystem.c \
	src/base/ftinit.c \
	src/base/ftdebug.c \
	src/base/ftbase.c \
	src/base/ftbbox.c \
	src/base/ftglyph.c \
	src/base/ftbitmap.c \
	src/sfnt/sfnt.c \
	src/truetype/truetype.c \
	src/raster/raster.c \
	src/smooth/smooth.c \
	src/autofit/autofit.c \
	src/gzip/ftgzip.c

LIBSTATIC_OBJECTS := $(addprefix obj/, $(addsuffix .o, $(basename $(SRC_FILES))))
OBJECTS_DIRS := $(sort $(dir $(LIBSTATIC_OBJECTS)))

CFLAGS += $(addprefix -I, $(C_INCLUDES))

all : dir_objects ../lib$(MODULE_NAME).a

../lib$(MODULE_NAME).a : $(LIBSTATIC_OBJECTS)
	$(AR) cr $@ $(LIBSTATIC_OBJECTS)

$(LIBSTATIC_OBJECTS) : obj/%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

dir_objects :
	-mkdir -p $(OBJECTS_DIRS)

clean :
	-rm -rf obj ../lib$(MODULE_NAME).a


#	Файлы исходников:
#
#	src/base/ftsystem.c
#	src/base/ftinit.c
#	src/base/ftdebug.c
#
#	src/base/ftbase.c
#
#	src/base/ftbbox.c       -- recommended, see <ftbbox.h>
#	src/base/ftglyph.c      -- recommended, see <ftglyph.h>
#
#	src/base/ftbdf.c        -- optional, see <ftbdf.h>
#	src/base/ftbitmap.c     -- optional, see <ftbitmap.h>
#	src/base/ftcid.c        -- optional, see <ftcid.h>
#	src/base/ftfntfmt.c     -- optional, see <ftfntfmt.h>
#	src/base/ftfstype.c     -- optional
#	src/base/ftgasp.c       -- optional, see <ftgasp.h>
#	src/base/ftgxval.c      -- optional, see <ftgxval.h>
#	src/base/ftlcdfil.c     -- optional, see <ftlcdfil.h>
#	src/base/ftmm.c         -- optional, see <ftmm.h>
#	src/base/ftotval.c      -- optional, see <ftotval.h>
#	src/base/ftpatent.c     -- optional
#	src/base/ftpfr.c        -- optional, see <ftpfr.h>
#	src/base/ftstroke.c     -- optional, see <ftstroke.h>
#	src/base/ftsynth.c      -- optional, see <ftsynth.h>
#	src/base/fttype1.c      -- optional, see <t1tables.h>
#	src/base/ftwinfnt.c     -- optional, see <ftwinfnt.h>
#
#	src/base/ftmac.c        -- only on the Macintosh
#
#	-- font drivers (optional; at least one is needed)
#
#	src/bdf/bdf.c           -- BDF font driver
#	src/cff/cff.c           -- CFF/OpenType font driver
#	src/cid/type1cid.c      -- Type 1 CID-keyed font driver
#	src/pcf/pcf.c           -- PCF font driver
#	src/pfr/pfr.c           -- PFR/TrueDoc font driver
#	src/sfnt/sfnt.c         -- SFNT files support
#	                         (TrueType & OpenType)
#	src/truetype/truetype.c -- TrueType font driver
#	src/type1/type1.c       -- Type 1 font driver
#	src/type42/type42.c     -- Type 42 font driver
#	src/winfonts/winfnt.c   -- Windows FONT / FNT font driver
#
#	-- rasterizers (optional; at least one is needed for vector
#	formats)
#
#	src/raster/raster.c     -- monochrome rasterizer
#	src/smooth/smooth.c     -- anti-aliasing rasterizer
#
#	-- auxiliary modules (optional)
#
#	src/autofit/autofit.c   -- auto hinting module
#	src/cache/ftcache.c     -- cache sub-system (in beta)
#	src/gzip/ftgzip.c       -- support for compressed fonts (.gz)
#	src/lzw/ftlzw.c         -- support for compressed fonts (.Z)
#	src/bzip2/ftbzip2.c     -- support for compressed fonts (.bz2)
#	src/gxvalid/gxvalid.c   -- TrueTypeGX/AAT table validation
#	src/otvalid/otvalid.c   -- OpenType table validation
#	src/psaux/psaux.c       -- PostScript Type 1 parsing
#	src/pshinter/pshinter.c -- PS hinting module
#	src/psnames/psnames.c   -- PostScript glyph names support
