#pragma once

#include <vector>

#include "particle.hh"
// Should remove dep on particle lib by making this generic,
// but putting constraints on the template type param.

// Assumption:
// - root node is over the area [0, 1] x [0, 1]
//   (could change this to ints over [0, 2^32))
// - leaf nodes contain only 0 or 1 particles
//   (could change this to contain a group, and index into a list of particles)
// - storing monopole info in non-leaf nodes

class QuadTree {
public:
  struct Node {
    Node *upper_left;
    Node *upper_right;
    Node *lower_left;
    Node *lower_right;

    bool leaf = true;

    // Null for non-leaf nodes and leaf nodes with no particle
    Particle *particle;

    // Center of charge and charge (monopole info, only for non-leaf nodes)
    // Also, could re-use the Particle* and point to a Particle with only these
    // fields.
    V2 coc{};
    int charge{};
  };

  QuadTree() = default;

  QuadTree(const std::vector<Particle *> &particles);

  void add(Particle *const particle);

  // To be used for get_temperature
  double get_potential(const Particle &particle);

  // Update or evolve tree (as particles move).
  void update();

  // Only used for testing.
  Node get_root() const;

private:
  Node root_;
};
