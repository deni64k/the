cc_library(
    name = "libcommon",
    srcs = glob([
        "common/**/*.cxx",
    ]),
    hdrs = glob([
        "common/**/*.hxx",
    ]),
    copts = [
        "-Wdouble-promotion",
        "-Iexternal/gsl/include",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@gsl//:main",
    ],
)

cc_library(
    name = "libui",
    srcs = glob([
        "ui/**/*.cxx",
    ]),
    hdrs = glob([
        "ui/**/*.hxx",
    ]),
    copts = [
        "-Wdouble-promotion",
        "-Iexternal/gsl/include",
        "-Iexternal/freetype/include",
    ],
    linkopts = select({
        "//the:darwin": [
            "-framework OpenGL",
            "-framework GLUT",
            "-lglfw",
            "-lglew",
        ],
        "//the:darwin_x86_64": [
            "-framework OpenGL",
            "-framework GLUT",
            "-lglfw",
            "-lglew",
        ],
        "//the:windows_msvc": [
            # XXX: ???
        ],
    }),
    data = glob([
        "ui/shaders/*.glsl",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        ":libcommon",
        "@gsl//:main",
        "@freetype//:libfreetype",
    ],
)
