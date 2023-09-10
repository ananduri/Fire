#include <cassert>
#include <cmath>
#include <iostream>

#include "quadtree.hh"

QuadTree::Node QuadTree::get_root() const { return root_; }

QuadTree::QuadTree(const std::vector<Particle *> &particles) {
  for (Particle *const particle : particles) {
    add(particle);
  }
}

void QuadTree::add(Particle *const particle) {
  Node *curr = &root_;
  V2 center{0.5, 0.5};
  double extent = 0.5;
  // bool added = false;
  const V2 position = particle->position;

  // Also need to propagate updates back up through tree.
  while (true) {
    if (curr->leaf) {
      if (!curr->particle) {
        curr->particle = particle;
        return;
      } else {
        // Make new child nodes.
        // Allocate them all now? Simpler code.
        // Test perf later w/o doing this.
        curr->upper_left = new Node();
        curr->upper_right = new Node();
        curr->lower_left = new Node();
        curr->lower_right = new Node();

        if (position.y > center.y && position.x < center.x) {
          curr->upper_left->particle = particle;
        } else if (position.y > center.y && position.x > center.x) {
          curr->upper_right->particle = particle;
        } else if (position.y < center.y && position.x < center.x) {
          curr->lower_left->particle = particle;
        } else {
          curr->lower_right->particle = particle;
        }
        curr->leaf = false;
        return;
      }
    } else {
      // Find child node to recurse into.
      // First, update monopole info.
      // But, this should only be updated if the particle is currently
      // combusting.
      if (std::holds_alternative<Combusting>(particle->state)) {
        const double energy = std::get<Combusting>(particle->state).energy;
        curr->charge += energy;
        curr->coc.x *= (curr->charge - energy) / curr->charge;
        curr->coc.x += energy * position.x / curr->charge;
        curr->coc.y *= (curr->charge - energy) / curr->charge;
        curr->coc.y += energy * position.y / curr->charge;
      }

      extent /= 2.;
      if (position.y > center.y && position.x < center.x) {
        curr = curr->upper_left;
        center.y += extent;
        center.x -= extent;
      } else if (position.y > center.y && position.x > center.x) {
        curr = curr->upper_right;
        center.y += extent;
        center.x += extent;
      } else if (position.y < center.y && position.x < center.x) {
        curr = curr->lower_left;
        center.y -= extent;
        center.x -= extent;
      } else {
        curr = curr->lower_right;
        center.y -= extent;
        center.x += extent;
      }
    }
  }
}

double QuadTree::get_temperature(const V2 &location) {
  // Write this recursively for now.
  double temperature = 0.;
  const auto f = [&](const Node &node, const V2 &center, const double extent,
                     auto &&f) -> void {
    // Is the node/square a near-field? I.e., does it border the particle's
    // square (at this depth)?
    if (node.leaf) {
      if (!node.particle) {
        return;
      }
      assert(node.particle != nullptr);

      temperature += temperature_contrib(location, *node.particle);
    } else if ((std::abs(location.x - center.x) <= 1.5 * extent) &&
               (std::abs(location.y - center.y) <= 1.5 * extent)) {
      const double new_extent = extent / 2.;
      f(*node.upper_left, V2{center.y + new_extent, center.x - new_extent},
        new_extent, f);
      f(*node.upper_right, V2{center.y + new_extent, center.x + new_extent},
        new_extent, f);
      f(*node.lower_left, V2{center.y - new_extent, center.x - new_extent},
        new_extent, f);
      f(*node.lower_right, V2{center.y - new_extent, center.x + new_extent},
        new_extent, f);
    } else {
      // Could reuse the particle repr.
      Particle monopole{
          .state = Combusting{node.charge},
          .position = node.coc,
          .speed_r = 0,
      };
      temperature += temperature_contrib(location, monopole);
    }
  };

  // Could "unroll" the first call here.
  f(root_, V2{0.5, 0.5}, 0.5, f);

  return temperature;
}
