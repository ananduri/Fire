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

TEST(QuadTreeTest, IntactParticleMeansZeroTemperature) {
  // SETUP
  QuadTree quadtree;
  V2 origin{0, 0};
  Particle particle{
      .state = Intact{},
      .position = origin,
      .speed_r = 0,
  };

  // ACTION
  quadtree.add(&particle);
  double temp = quadtree.get_temperature(origin);

  // VERIFICATION
  EXPECT_EQ(temp, 0.);
}

TEST(QuadTreeTest, CombustingParticleMeansCappedTemperatureAtSameLocation) {
  // SETUP
  QuadTree quadtree;
  V2 origin{0, 0};
  Particle particle{
      .state = Combusting{1},
      .position = origin,
      .speed_r = 0,
  };

  // ACTION
  quadtree.add(&particle);
  double temp = quadtree.get_temperature(origin);

  // VERIFICATION
  EXPECT_EQ(temp, 1 / CUTOFF_M);
}
