#pragma once

#include <variant>

// What need to do in order to emplace this again? Need ctor?
struct V2 {
  double x = 0;
  double y = 0;
};

struct Intact {};

struct Combusting {
  double energy = 0;
};

// Can use a std::variant for State
struct Particle {
  std::variant<Intact, Combusting> state;
  V2 position;
  double speed_r = 0;
};

double temperature_contrib(const V2 &location, const Particle &particle);
