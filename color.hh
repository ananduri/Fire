#pragma once

#include "particle.hh"

static constexpr double P_SIZE = 1.0;

// Might want to use HSL instead? Look into it.
struct HSV {
  double hue = 0;
  double sat = 0;
  double val = 0;
};

// The Planck law gives the entire spectrum as a function of temperature.
// Need a way to map temperature directly into color space.
// Can we map from spectrum into color space?
// Maybe spectrum to mean or peak frequency, then to color space?
HSV blackbody(const double temp) {
  // Implement.
  return {};
}

HSV getColor(const double x, const double y, const Particle &particle) {
  // Make a macro/template for squaring functions, pow is slow
  const double r_sq = (x - particle.position.x) * (x - particle.position.x) +
                      (y - particle.position.y) * (y - particle.position.y);
  if (std::holds_alternative<Intact>(particle.state)) {
    if (r_sq < P_SIZE) {
      return HSV{};
    }
    return HSV{};
  }
  // Particle is combusting.
  // Buuut, the color depends on the temperature which depends on all of the
  // particles' energy.
  return HSV{};
}
