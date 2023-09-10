#pragma once

#include <variant>

// Distance cutoff for evaluating potentials.
constexpr double CUTOFF_M = 1e-3;

struct V2 {
  V2() = default;
  V2(const double x, const double y) : x{x}, y{y} {}
  double x = 0;
  double y = 0;
};

struct Intact {};

struct Combusting {
  double energy = 0;
};

struct Particle {
  std::variant<Intact, Combusting> state;
  V2 position;
  double speed_r = 0;
};

double temperature_contrib(const V2 &location, const Particle &particle);
