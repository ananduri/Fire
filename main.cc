#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>

#include "particle.hh"
#include "quadtree.hh"

std::random_device rd;
std::mt19937 gen(rd());
static constexpr double PI = 3.141592653;
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

std::vector<V2> genInitialPositions() {
  // Fictional coordinate system has (0, 0) at the bottom left,
  // and (1, 1) in the top right.
  constexpr double R_UPPER = 0.5;
  std::uniform_real_distribution<> radius_dis(0, R_UPPER);
  std::uniform_real_distribution<> angle_dis(0, 2 * PI);

  constexpr int NUM_PARTICLES = 100;
  std::vector<V2> positions;
  for (int i = 0; i < NUM_PARTICLES; ++i) {
    double r = radius_dis(gen);
    double a = angle_dis(gen);
    positions.emplace_back(r * std::cos(a) + 0.5, r * std::sin(a) + 0.5);
  }
  return positions;
}

// Convert from pixel coordinates ([0..width]x[0..width])
// to the [0,1]x[0,1] coordinate frame.
double pix2pic(const int i, const int width) {
  return static_cast<double>(i) / width;
}

int main() {
  // Initial conditions
  std::vector<V2> positions = genInitialPositions();
  // std::vector<V2> positions{V2{0.5, 0.5}, V2{0.7, 0.7}, V2{0.7, 0.7}};
  // Insert particles into data structure of choice.
  std::vector<Particle *> particles;
  std::transform(positions.cbegin(), positions.cend(),
                 std::back_inserter(particles), [](const V2 &position) {
                   return new Particle{
                       .state = Combusting{1.},
                       .position = position,
                       .speed_r = 0,
                   };
                 });
  QuadTree quadtree{particles};

  // Render.
  // Can parallelize this eventually.
  constexpr int WIDTH = 255;
  constexpr int HEIGHT = 255;

  std::cout << "P3\n" << WIDTH << ' ' << HEIGHT << "\n255\n";

  std::vector<double> temps;

  for (int j = HEIGHT - 1; j >= 0; --j) {
    for (int i = 0; i < WIDTH; ++i) {
      // Get R, G, B from coords (i, j)
      // Get HSV first, then convert to RGB
      double x = pix2pic(i, WIDTH);
      double y = pix2pic(j, HEIGHT);

      // // Just have one extra loop outside for rendering the particles.
      // // Or, just skip rendering the particles.
      // // Actually, this was just for testing.
      // bool in_particle = false;
      // for (const auto &p : particles) {
      //   if (pow(x - p.position.x, 2) + pow(y - p.position.y, 2) < 1) {
      //     in_particle = true;
      //     break;
      //   }
      // }
      // int ir = in_particle ? 255 : 20;
      // int ig = in_particle ? 255 : 20;
      // int ib = in_particle ? 255 : 20;

      auto temp = quadtree.get_temperature(V2{x, y});
      temps.push_back(temp);

      // std::cerr << temp << std::endl;

      // Testing.
      // These vars scale from 0 to 1.
      // auto r = temp / 10000.;
      auto r = std::tanh(temp / 10000.);
      auto g = 0.;
      auto b = 0.;

      // Int versions.
      int ir = static_cast<int>(r * 255.999);
      int ig = static_cast<int>(g * 255.999);
      int ib = static_cast<int>(b * 255.999);

      std::cout << ir << ' ' << ig << ' ' << ib << '\n';
    }
  }
  std::cerr << "max temp: " << *std::max_element(temps.cbegin(), temps.cend())
            << std::endl;
}
