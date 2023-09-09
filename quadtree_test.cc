#include <gtest/gtest.h>

#include "particle.hh"
#include "quadtree.hh"

TEST(QuadTreeTest, WhenAddParticleRootChildrenShouldBeNonNull) {
  // SETUP
  QuadTree quadtree;
  Particle particle;

  // ACTION
  quadtree.add(&particle);

  // VERIFICATION
  const QuadTree::Node root = quadtree.get_root();
  EXPECT_NE(root.upper_left, nullptr);
  EXPECT_NE(root.upper_right, nullptr);
  EXPECT_NE(root.lower_left, nullptr);
  EXPECT_NE(root.lower_right, nullptr);
}
