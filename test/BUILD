cc_test(
    name = "main",
    timeout = "short",
    srcs = glob(["*.cxx"]),
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "//lib:libastrolabe",
        "@gtest//:main",
    ],
)
