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
    deps = [
        "//lib:libastrolabe",
        "@freetype//:libfreetype",
    ],
)
