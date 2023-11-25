#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <sstream>
//#include <format>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

#include "color.hh"
#include "particle.hh"
#include "quadtree.hh"

std::random_device rd;
std::mt19937 gen(rd());
static constexpr double PI = 3.141592653;
static constexpr double TIMESTEP = 0.1;

std::vector<V2> genInitialPositions() {
  // Fictional coordinate system has (0, 0) at the bottom left,
  // and (1, 1) in the top right.
  constexpr double R_UPPER = 0.5;
  std::uniform_real_distribution<> radius_dis(0, R_UPPER);
  std::uniform_real_distribution<> angle_dis(0, 2 * PI);

  constexpr int NUM_PARTICLES = 200;
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

void step_particles(std::vector<Particle *> &particles) {
  // Have timestep controlled via CLI flags (number & spacing)
  for (Particle *p : particles) {
    double const x_from_center = p->position.x - 0.5;
    double const y_from_center = p->position.y - 0.5;
    double const radial_dist =
        sqrt(x_from_center * x_from_center + y_from_center * y_from_center);
    double const new_radial_dist = radial_dist + p->speed_r * TIMESTEP;
    p->position.x = (new_radial_dist / radial_dist) * x_from_center + 0.5;
    p->position.y = (new_radial_dist / radial_dist) * y_from_center + 0.5;
  }
}

// Can add parellelism here.
void render_particles(std::vector<Particle *> const &particles,
                      QuadTree const &quadtree, int const width,
                      int const height, bool const use_quadtree,
                      int const index) {
  std::ofstream file_handle;
  // file_handle.open(std::format("output/img_{}", index));
  // Do I have to use the absolute path? Is it because of Bazel?
  std::stringstream ss;
  ss << "/Users/drake/Documents/projects/genart/fire_cpp/output/img_" << index
     << ".ppm";
  file_handle.open(ss.str());
  if (!file_handle.is_open()) {
    std::cout << "file not open\n";
    return;
  }
  file_handle << "P3\n" << width << ' ' << height << "\n255\n";

  std::vector<double> temps;
  auto const time_start_render = std::chrono::steady_clock::now();
  for (int j = height - 1; j >= 0; --j) {
    for (int i = 0; i < width; ++i) {
      // Get R, G, B from coords (i, j)
      // Get HSV first, then convert to RGB
      double x = pix2pic(i, width);
      double y = pix2pic(j, height);

      double const temp = [&]() {
        V2 const loc{x, y};
        if (use_quadtree) {
          return quadtree.get_temperature(loc);
        } else {
          double temp = 0.;
          for (Particle *const p : particles) {
            temp += temperature_contrib(loc, *p);
          }
          return temp;
        }
      }();
      temps.push_back(temp);

      // These vars scale from 0 to 1.
      auto r = std::tanh(temp / 100.);
      auto g = std::tanh(temp / 100000.);
      auto b = 0.75 * (1 - std::tanh(temp / 10000.));

      // Int versions.
      int ir = static_cast<int>(r * 255.999);
      int ig = static_cast<int>(g * 255.999);
      int ib = static_cast<int>(b * 255.999);

      file_handle << ir << ' ' << ig << ' ' << ib << '\n';
    }
  }
  file_handle.close();
  auto const time_end_render = std::chrono::steady_clock::now();
  std::chrono::duration<double> const diff =
      time_end_render - time_start_render;
  std::cout << "time to render: " << diff.count() << '\n';
  std::cout << "max temp: " << *std::max_element(temps.cbegin(), temps.cend())
            << '\n';
}

int main(int argc, char *argv[]) {
  // CLI flags
  bool use_quadtree = true;
  if (argc > 1) {
    char const opt = *argv[1];
    std::cout << opt << '\n';
    if (opt == 'q')
      use_quadtree = true;
    else if (opt == 'n')
      use_quadtree = false;
  }

  // Add CLI option for number of pixels as well
  constexpr int WIDTH = 255 * 2;
  constexpr int HEIGHT = 255 * 2;

  // Initial conditions
  std::vector<V2> positions = genInitialPositions();
  std::vector<Particle *> particles;
  std::transform(positions.cbegin(), positions.cend(),
                 std::back_inserter(particles), [](const V2 &position) {
                   return new Particle{
                       .state = Combusting{1.},
                       .position = position,
                       .speed_r = 0,
                   };
                 });
  // Time this as well.
  QuadTree quadtree{particles};

  // Add time evolution.
  // Do it using the quadtree after adding remove() to it.

  for (int timestep = 0; timestep < 5; ++timestep) {
    render_particles(particles, quadtree, WIDTH, HEIGHT, use_quadtree,
                     timestep);
    step_particles(particles);
  }

  // Run post-processing step to create the MP4? Or do it in CLI?
  std::system(
      "ffmpeg -r 10 -i "
      "/Users/drake/Documents/projects/genart/fire_cpp/output/img_%d.ppm -c:v "
      "libx264 -crf 25 -vf 'scale=500:500,format=yuv420p' -movflags +faststart "
      "/Users/drake/Documents/projects/genart/fire_cpp/output/output.mp4");
}
