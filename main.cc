#include <algorithm>
#include <cmath>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>

#include "color.hh"
#include "particle.hh"
#include "quadtree.hh"

std::random_device rd;
std::mt19937 gen(rd());
static constexpr double PI = 3.141592653;

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
  constexpr int WIDTH = 255 * 2;
  constexpr int HEIGHT = 255 * 2;

  std::cout << "P3\n" << WIDTH << ' ' << HEIGHT << "\n255\n";

  std::vector<double> temps;

  auto const time_start_render = std::chrono::steady_clock::now();
  for (int j = HEIGHT - 1; j >= 0; --j) {
    for (int i = 0; i < WIDTH; ++i) {
      // Get R, G, B from coords (i, j)
      // Get HSV first, then convert to RGB
      double x = pix2pic(i, WIDTH);
      double y = pix2pic(j, HEIGHT);

      // Macro or CLI arg for switching btw these
      //auto temp = quadtree.get_temperature(V2{x, y});
      
      double temp = 0.;
      V2 const loc{x, y};
      for (Particle* const p : particles) {
        temp += temperature_contrib(loc, *p);
      }
      temps.push_back(temp);
      
      // std::cerr << temp << std::endl;

      // Testing.
      // These vars scale from 0 to 1.
      // auto r = temp / 10000.;
      auto r = std::tanh(temp / 100.);
      auto g = std::tanh(temp / 100000.);
      auto b = 0.75 * (1 - std::tanh(temp / 10000.));

      // Int versions.
      int ir = static_cast<int>(r * 255.999);
      int ig = static_cast<int>(g * 255.999);
      int ib = static_cast<int>(b * 255.999);

      std::cout << ir << ' ' << ig << ' ' << ib << '\n';
    }
  }
  auto const time_end_render = std::chrono::steady_clock::now();
  std::chrono::duration<double> const diff = time_end_render - time_start_render;
  std::cerr << "time to render: " << diff.count() << '\n';
  
  std::cerr << "max temp: " << *std::max_element(temps.cbegin(), temps.cend())
            << std::endl;
}
