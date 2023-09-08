#pragma once

#include <vector>
#include <memory>

// Can generalize the type of the mono/di/quad/etc-poles to Pole<N> where N is the order (0, 1, 2, etc.)
// Pole<N> will contain a rank-N tensor

struct Node {
  Monopole q;
  std::vector<std::unique_ptr<Node>> children;
  // Need pointer to parent? not sure. maybe can get away without.
};

struct Monopole {
  double charge = 0;
  V2 position;
};

// Simple case of only containing/approximating as monopoles.
class MonopoleTree {
 public:
  MonopoleTree() = default;
  
  // How to generalize this for any container again?
  MonopoleTree(std::vector<Monopole> monopoles) {
    //
  }
  
  void addCharge(const Monopole& monopole) {}
  
  double getPotential(const V2& position) {}
  
 private:
  Node head;
};
