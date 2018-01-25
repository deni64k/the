workspace(name = "the")

load("//tools/bazel_ext:patched_http_archive.bzl", "patched_http_archive")

new_http_archive(
    name = "gmp",
    url = "https://ftp.gnu.org/gnu/gmp/gmp-6.1.2.tar.xz",
    sha256 = "87b565e89a9a684fe4ebeeddb8399dce2599f9c9049854ca8c0dfbdea0e21912",
    # strip_prefix = "gmp-6.1.0",
    build_file_content = "filegroup(name = \"top\", srcs = [\"gmp-6.1.2\"], visibility = [\"//visibility:public\"])",
)

new_http_archive(
    name = "mpfr",
    url = "https://ftp.gnu.org/gnu/mpfr/mpfr-4.0.0.tar.xz",
    sha256 = "fbe2cd1418b321f5c899ce4f0f0f4e73f5ecc7d02145b0e1fd096f5c3afb8a1d",
    # strip_prefix = "mpfr-3.1.4",
    build_file_content = "filegroup(name = \"top\", srcs = [\"mpfr-4.0.0\"], visibility = [\"//visibility:public\"])",
)

new_http_archive(
    name = "mpc",
    url = "https://ftp.gnu.org/gnu/mpc/mpc-1.1.0.tar.gz",
    sha256 = "6985c538143c1208dcb1ac42cedad6ff52e267b47e5f970183a3e75125b43c2e",
    # strip_prefix = "mpc-1.0.3",
    build_file_content = "filegroup(name = \"top\", srcs = [\"mpc-1.1.0\"], visibility = [\"//visibility:public\"])",
)

new_http_archive(
    name = "isl",
    url = "http://gcc.gnu.org/pub/gcc/infrastructure/isl-0.18.tar.bz2",
    sha256 = "6b8b0fd7f81d0a957beb3679c81bbb34ccc7568d5682844d8924424a0dadcb1b",
    # strip_prefix = "isl-0.18",
    build_file_content = "filegroup(name = \"top\", srcs = [\"isl-0.18\"], visibility = [\"//visibility:public\"])",
)

new_http_archive(
    name = "binutils",
    # url = "https://ftp.gnu.org/gnu/binutils/binutils-2.29.1.tar.xz",
    # sha256 = "e7010a46969f9d3e53b650a518663f98a5dde3c3ae21b7d71e5e6803bc36b577",
    url = "https://ftp.gnu.org/gnu/binutils/binutils-2.28.tar.bz2",
    sha256 = "6297433ee120b11b4b0a1c8f3512d7d73501753142ab9e2daa13c5a3edd32a72",
    # build_file_content = "filegroup(name = \"top\", srcs = [\"binutils-2.29.1\"], visibility = [\"//visibility:public\"])",
    build_file_content = "filegroup(name = \"top\", srcs = [\"binutils-2.28\"], visibility = [\"//visibility:public\"])",
)

new_http_archive(
    name = "gcc",
    url = "https://ftp.gnu.org/gnu/gcc/gcc-7.3.0/gcc-7.3.0.tar.xz",
    sha256 = "832ca6ae04636adbb430e865a1451adf6979ab44ca1c8374f61fba65645ce15c",
    build_file_content = "filegroup(name = \"top\", srcs = [\"gcc-7.3.0\"], visibility = [\"//visibility:public\"])",
    # build_file = "gcc.BUILD",
)

new_http_archive(
    name = "gtest",
    url = "https://github.com/google/googletest/archive/release-1.7.0.zip",
    sha256 = "b58cb7547a28b2c718d1e38aee18a3659c9e3ff52440297e965f5edffe34b6d0",
    build_file = "gtest.BUILD",
    strip_prefix = "googletest-release-1.7.0",
)

new_http_archive(
    name = "gsl",
    url = "https://github.com/martinmoene/gsl-lite/archive/v0.24.0.zip",
    sha256 = "0c982fefd412156a6a69fe932e39ca2a84b0ad6a328e2fca8bf3646421f79421",
    # url = "https://github.com/Microsoft/GSL/archive/2b8d204.zip",
    # sha256 = "2739529395632e644815a7980092c48abb1f93f49ec37d911c4d6964e9069e05",
    build_file = "gsl.BUILD",
    # strip_prefix = "GSL-2b8d20425e990c5a3e9a0158e2cedacbcdf9e522",
    strip_prefix = "gsl-lite-0.24.0",
)

new_http_archive(
    name = "freetype",
    url = "https://download.savannah.gnu.org/releases/freetype/freetype-2.8.1.tar.bz2",
    sha256 = "e5435f02e02d2b87bb8e4efdcaa14b1f78c9cf3ab1ed80f94b6382fb6acc7d78",
    build_file = "freetype.BUILD",
    strip_prefix = "freetype-2.8.1",
)
