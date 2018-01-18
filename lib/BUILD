cc_library(
    name = "libastrolabe",
    srcs = glob([
        "**/*.cxx",
    ]),
    hdrs = glob([
        "**/*.hxx",
    ]),
    copts = [
        "-Wdouble-promotion",
        "-Iexternal/gsl/include",
        "-Iexternal/freetype/include",
    ],
    linkopts = select({
        ":darwin": [
            "-framework OpenGL",
            "-framework GLUT",
            "-lglfw",
            "-lglew",
        ],
        ":darwin_x86_64": [
            "-framework OpenGL",
            "-framework GLUT",
            "-lglfw",
            "-lglew",
        ],
        ":windows_msvc": [
            # XXX: ???
        ],
    }),
    data = glob([
        "shaders/*.glsl",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "@gsl//:main",
        "@freetype//:libfreetype",
    ],
)

config_setting(
    name = "darwin",
    values = {"cpu": "darwin"},
    visibility = ["//visibility:public"],
)

config_setting(
    name = "darwin_x86_64",
    values = {"cpu": "darwin_x86_64"},
    visibility = ["//visibility:public"],
)

config_setting(
    name = "iphonesdk",
    values = {"define": "IPHONE_SDK=1"},
    visibility = ["//visibility:public"],
)

config_setting(
    name = "freebsd",
    values = {"cpu": "freebsd"},
    visibility = ["//visibility:public"],
)

config_setting(
    name = "windows",
    values = {"cpu": "x64_windows"},
    visibility = ["//visibility:public"],
)

config_setting(
    name = "windows_msvc",
    values = {"cpu": "x64_windows_msvc"},
    visibility = ["//visibility:public"],
)
