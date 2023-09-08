#include <cmath>
#include <iostream>
#include <random>
#include <variant>
#include <vector>

std::random_device rd;
std::mt19937 gen(rd());
static constexpr double PI = 3.141592653;
static constexpr double P_SIZE = 1.0;

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
    return;
  }
  // Particle is combusting.
  // Buuut, the color depends on the temperature which depends on all of the
  // particles' energy.
}

std::vector<V2> genInitialPositions() {
  // Fictional coordinate system has (0,0) at the center,
  // and stretches to 100 in each direction (so goes from -100 to 100 in x, same
  // in y). const double r_upper = 1.5;
  const double r_upper = 50;
  std::uniform_real_distribution<> radius_dis(0, r_upper);
  std::uniform_real_distribution<> angle_dis(0, 2 * PI);

  // Insert particles into data structure of choice.
  std::vector<V2> positions;
  for (int i = 0; i < 10; ++i) {
    double r = radius_dis(gen);
    double a = angle_dis(gen);
    positions.push_back(V2{r * std::cos(a), r * std::sin(a)});
  }
  return positions;
}

double pix2pic(const int i, const int width) {
  const double p = 1.0 - (1.0 / width);
  const double a = (width - 1) / 2.0;
  return (100.0 / a) * p * (i - a);
}

int main() {
  // Initial conditions
  std::vector<V2> positions = genInitialPositions();
  std::vector<Particle> particles;
  std::transform(positions.cbegin(), positions.cend(),
                 std::back_inserter(particles), [](const V2 &position) {
                   return Particle{
                       .state = Intact{},
                       .position = position,
                       .speed_r = 0,
                   };
                 });

  // Render function
  // Can parallelize this eventually.
  const int width = 255;
  const int height = 255;

  std::cout << "P3\n" << width << ' ' << height << "\n255\n";

  for (int j = height - 1; j >= 0; --j) {
    for (int i = 0; i < width; ++i) {
      // Get R, G, B from coords (i, j)
      // Get HSV first, then convert to RGB
      double x = pix2pic(i, width);
      double y = pix2pic(j, width);

      bool in_particle = false;
      for (const auto &p : particles) {
        if (pow(x - p.position.x, 2) + pow(y - p.position.y, 2) < 1) {
          in_particle = true;
          break;
        }
      }

      int ir = in_particle ? 255 : 20;
      int ig = in_particle ? 255 : 20;
      int ib = in_particle ? 255 : 20;

      // auto r = double(i) / (width - 1);
      // auto g = 0.25;
      // auto b = double(j) / (height - 1);

      // int ir = static_cast<int>(r * 255.999);
      // int ig = static_cast<int>(g * 255.999);
      // int ib = static_cast<int>(b * 255.999);

      std::cout << ir << ' ' << ig << ' ' << ib << '\n';
    }
  }
}
