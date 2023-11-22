cc_binary(
  name = "fire",
  srcs = [
    "main.cc",
  ],
  deps = [
    ":particle",
    ":quadtree",
    ":color",
  ],
)

cc_library(
  name = "color",
  hdrs = ["color.hh"],
)


cc_library(
  name = "particle",
  hdrs = ["particle.hh"],
  srcs = ["particle.cc"],
)

cc_library(
  name = "quadtree",
  srcs = ["quadtree.cc"],
  hdrs = ["quadtree.hh"],
  deps = [
    ":particle",
  ]
)

cc_test(
  name = "quadtree_test",
  size = "small",
  srcs = ["quadtree_test.cc"],
  deps = [
    ":quadtree",
    ":particle",
    "@com_google_googletest//:gtest_main",
  ]
)
