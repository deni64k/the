cc_library(
    name = "libastrolabe",
    srcs = glob([
        "**/*.cxx",
    ]),
    hdrs = glob([
        "**/*.hxx",
    ]),
    copts = [
        "-fno-exceptions",
        "-Wdouble-promotion",
        "-Iexternal/gsl/include",
        "-Iexternal/freetype/include",
    ],
    data = glob([
        "shaders/*.glsl",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "@gsl//:main",
        "@freetype//:libfreetype",
    ],
)
