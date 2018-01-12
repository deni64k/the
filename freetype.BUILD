cc_library(
    name = "libfreetype",
    srcs = [
        # auto hinting module
        "src/autofit/autofit.c",
        "src/base/ftbase.c",
        # recommended, see <ftbbox.h>
        "src/base/ftbbox.c",
        # optional, see <ftbdf.h>
        "src/base/ftbdf.c",
        # optional, see <ftbitmap.h>
        "src/base/ftbitmap.c",
        # optional, see <ftcid.h>
        "src/base/ftcid.c",
        "src/base/ftdebug.c",
        # optional, see <ftfntfmt.h>
        "src/base/ftfntfmt.c",
        # optional
        "src/base/ftfstype.c",
        # optional, see <ftgasp.h>
        "src/base/ftgasp.c",
        # recommended, see <ftglyph.h>
        "src/base/ftglyph.c",
        # optional, see <ftgxval.h>
        "src/base/ftgxval.c",
        "src/base/ftinit.c",
        # optional, see <ftlcdfil.h>
        "src/base/ftlcdfil.c",
        # optional, see <ftmm.h>
        "src/base/ftmm.c",
        # optional, see <ftotval.h>
        "src/base/ftotval.c",
        # optional
        "src/base/ftpatent.c",
        # optional, see <ftpfr.h>
        "src/base/ftpfr.c",
        # optional, see <ftstroke.h>
        "src/base/ftstroke.c",
        # optional, see <ftsynth.h>
        "src/base/ftsynth.c",
        "src/base/ftsystem.c",
        # optional, see <t1tables.h>
        "src/base/fttype1.c",
        # optional, see <ftwinfnt.h>
        "src/base/ftwinfnt.c",

        # drivers
        "src/bdf/bdfdrivr.c",
        "src/cff/cff.c",
        "src/pcf/pcf.c",
        "src/pfr/pfr.c",
        "src/sfnt/sfnt.c",
        "src/type1/type1.c",
        "src/cid/type1cid.c",
        "src/type42/type42.c",
        "src/truetype/truetype.c",
        "src/winfonts/winfnt.c",

        # cache sub-system (in beta)
        "src/cache/ftcache.c",
        # TrueTypeGX/AAT table validation
        "src/gxvalid/gxvalid.c",
        # OpenType table validation
        "src/otvalid/otvalid.c",
        # PostScript Type 1 parsing
        "src/psaux/psaux.c",
        # PS hinting module
        "src/pshinter/pshinter.c",
        # PostScript glyph names support
        "src/psnames/psnames.c",
        # monochrome rasterizer
        "src/raster/raster.c",
        # anti-aliasing rasterizer
        "src/smooth/smooth.c",

        # support for compressed fonts (.bz2)
        "src/bzip2/ftbzip2.c",
        # support for compressed fonts (.gz)
        "src/gzip/ftgzip.c",
        # support for compressed fonts (.Z)
        "src/lzw/ftlzw.c",
    ],
    hdrs = glob(
        [
            "include/**/*.h",
        ],
    ),
    copts = ["-DFT2_BUILD_LIBRARY"],
    includes = ["include"],
    textual_hdrs = glob([
        "src/**/*.h",
        "src/**/*.c",
    ]),
    alwayslink = 1,
    visibility = ["//visibility:public"],
)
