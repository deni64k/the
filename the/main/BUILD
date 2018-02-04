cc_binary(
    name = "coco",
    srcs = glob([
        "coco.cxx",
    ]),
    deps = [
        "//the/lib:libcommon",
    ],
)

cc_binary(
    name = "star",
    srcs = glob([
        "star.cxx",
    ]),
    copts = ["-fextended-identifiers"],
    deps = [
        "//the/lib:libcommon",
    ],
)

cc_binary(
    name = "starsky",
    srcs = glob([
        "starsky.cxx",
    ]),
    deps = [
        "//the/lib:libcommon",
        "//the/lib:libui",
        "@freetype//:libfreetype",
    ],
)
