#include <iostream>
#include <vector>
#include <variant>
#include <random>
#include <cmath>

std::random_device rd;
std::mt19937 gen(rd());
static constexpr double PI = 3.141592653;

// What need to do in order to emplace this again?
struct V2 {
  double x = 0;
  double y = 0;
};

struct Intact{};

struct Combusting {
  double energy = 0;
};

// Can use a std::variant for State
struct Particle {
  std::variant<Intact, Combusting> state;
  V2 position;
  double speed_r = 0;
};

std::vector<V2> genInitialPositions() {
  // Fictional coordinate system has (0,0) at the center,
  // and stretches to 100 in each direction (so goes from -100 to 100 in u, same in v).
  double r_upper = 1.5;
  std::uniform_real_distribution<> radius_dis(0, r_upper);
  std::uniform_real_distribution<> angle_dis(0, 2*PI);
  
  std::vector<V2> positions;
  for (int i = 0; i < 300; ++i) {
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
  std::transform(positions.cbegin(),                 
                 positions.cend(),
                 std::back_inserter(particles),
                 [](const V2& position) {
                   return Particle{
                     .state = Intact{},
                     .position = position,
                     .speed_r = 0,
                   };
                 });
  
  // Render function
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
      for (const auto& p : particles) {
        if (pow(x - p.position.x, 2) + pow(y - p.position.y, 2) < 1) {
          in_particle = true;
          break;
        }
      }
      
      int ir = in_particle ? 255 : 10;
      int ig = in_particle ? 255 : 10;
      int ib = in_particle ? 255 : 10;
      
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

