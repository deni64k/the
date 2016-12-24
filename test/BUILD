cc_test(
    name = "main",
    srcs = glob(["*.cxx"]),
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
        "//lib:libastrolabe",
    ],
    timeout="short",
)
