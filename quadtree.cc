#include "quadtree.hh"

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

// double get_potential(const Particle &particle) {
//   // Write this recursively for now.
//   double potential = 0.;
//   const auto f = [&](const Node *const node) {
//     // Is the node/square a far-field?
//   };
// }
