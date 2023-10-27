#ifndef RT_WEEKEND_H
#define RT_WEEKEND_H

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>

// Usings

using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// Constants

constexpr double infinity = std::numeric_limits<double>::infinity();
constexpr double pi = 3.1415926535897932385;

// Utility Functions

inline constexpr double deg_to_rad(double degrees) { return degrees * pi / 180.0; }

inline double random_double() { return rand() / (RAND_MAX + 1.0); }

inline double random_double(double min, double max) { return min + (max - min) * random_double(); }

// Common Headers

#include "interval.hpp"
#include "ray.hpp"
#include "vector3d.hpp"

#endif  // RT_WEEKEND_H