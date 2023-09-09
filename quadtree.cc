#include <cmath>

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
      curr->charge += 1;
      curr->coc.x *= (curr->charge - 1) / curr->charge;
      curr->coc.x += position.x / curr->charge + 1;

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
  const auto f = [&](const Node *const node, V2 center, double extent,
                     auto &&f) -> void {
    // Is the node/square a near-field? Does it border the particle's square (at
    // this depth)?
    if (node->leaf) {
      temperature += temperature_contrib(location, *node->particle);
    } else if ((std::abs(location.x - center.x) <= 1.5 * extent) &&
               (std::abs(location.y - center.y) <= 1.5 * extent)) {
      extent /= 2.;
      f(node->upper_left, V2{center.y + extent, center.x - extent}, extent, f);
      f(node->upper_right, V2{center.y + extent, center.x + extent}, extent, f);
      f(node->lower_left, V2{center.y - extent, center.x - extent}, extent, f);
      f(node->lower_right, V2{center.y - extent, center.x + extent}, extent, f);
    } else {
      // Could reuse the particle repr.
      Particle monopole{
          .state = Combusting{node->charge},
          .position = node->coc,
          .speed_r = 0,
      };
      temperature += temperature_contrib(location, monopole);
    }
  };

  // Could "unroll" the first call here.
  f(&root_, V2{0.5, 0.5}, 0.5, f);

  return temperature;
}
