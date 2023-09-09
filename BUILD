cc_binary(
  name = "fire",
  srcs = [
    "main.cc",
  ],
  deps = [
    ":particle",
    ":quadtree",
  ],
)

cc_library(
  name = "particle",
  hdrs = ["particle.hh"],
)

cc_library(
  name = "quadtree",
  srcs = ["quadtree.cc"],
  hdrs = ["quadtree.hh"],
  deps = [
    ":particle",
  ]
)
