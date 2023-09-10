#include "particle.hh"

double temperature_contrib(const V2 &location, const Particle &particle) {
  const double dist_sq =
      (location.x - particle.position.x) * (location.x - particle.position.x) +
      (location.y - particle.position.y) * (location.y - particle.position.y);
  if (std::holds_alternative<Combusting>(particle.state)) {
    const double energy = std::get<Combusting>(particle.state).energy;
    // Cap temp if dist_sq is below some threshold.
    if (dist_sq < CUTOFF_M) {
      return energy / CUTOFF_M;
    }
    return energy / dist_sq;
  } else {
    return 0;
  }
}
