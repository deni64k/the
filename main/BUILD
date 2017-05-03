cc_binary(
    name = "coco",
    srcs = glob([
         "coco.cxx",
    ]),
    deps = [
        "//lib:libastrolabe",
    ],
)

cc_binary(
    name = "star",
    srcs = glob([
         "star.cxx",
    ]),
    copts = ["-fextended-identifiers"],
    deps = [
        "//lib:libastrolabe",
    ],
)

cc_binary(
    name = "starsky",
    srcs = glob([
         "starsky.cxx",
    ]),
    # copts = ["-fno-exceptions"],
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
        ],
    }),
    deps = [
        "//lib:libastrolabe",
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
