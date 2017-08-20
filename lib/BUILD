cc_library(
    name = "libastrolabe",
    copts = [
        "-Wdouble-promotion",
        "-Iexternal/gsl/include",
    ],
    srcs = glob([
        "*.cxx",
    ]),
    hdrs = glob([
        "*.hxx",
    ]),
    data = glob([
        "shaders/*.glsl",
    ]),
    deps = [
        "@gsl//:main",
    ],
    visibility = ["//visibility:public"],
)
