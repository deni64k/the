package(default_visibility = ["//visibility:public"])

# cc_library(
#     name = "malloc",
# )

# cc_library(
#     name = "stl",
# )

filegroup(
    name = "empty",
    srcs = ["osx_cc_wrapper.sh"],
)

# Hardcoded toolchain, legacy behaviour.
cc_toolchain_suite(
    name = "toolchain",
    toolchains = {
        "darwin|gcc7": ":gcc7-compiler-darwin",
    },
)

cc_toolchain(
    name = "gcc7-compiler-darwin",
    all_files = ":empty",
    compiler_files = ":empty",
    cpu = "darwin",
    dwp_files = ":empty",
    dynamic_runtime_libs = [":empty"],
    linker_files = ":empty",
    objcopy_files = ":empty",
    static_runtime_libs = [":empty"],
    strip_files = ":empty",
    supports_param_files = 0,
)
