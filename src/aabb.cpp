
#include "aabb.h"

void AABB::clear() {
  const float inf = std::numeric_limits<float>::infinity();
  min.setConstant(inf);
  max.setConstant(-inf);
}

bool AABB::contains(const Eigen::Vector3f &p) {
  return (p.array() >= min.array()).all() &&
         (p.array() <= max.array()).all();
}

float AABB::distanceTo(const Eigen::Vector3f &p) {
  float result = 0;
  for (int i = 0; i < 3; ++i) {
    float value = 0;
    if (p[i] < min[i]) {
      value = min[i] - p[i];
    }
    else if (p[i] > max[i]) {
      value = p[i] - max[i];
    }
    result += value * value;
  }
  result = std::sqrt(result);
  return result;
}

void AABB::expandBy(const Eigen::Vector3f &p) {
  min = min.cwiseMin(p);
  max = max.cwiseMax(p);
}

float AABB::surfaceArea() {
  Eigen::Vector3f d = max - min;
  return 2.0f * (d[0] * d[1] + d[0] * d[2] + d[1] * d[2]);
}

Eigen::Vector3f AABB::center() {
  return 0.5f * (min + max);
}

// static AABB merge(const AABB &aabb1, const AABB &aabb2) {
//   return AABB(aabb1.min.cwiseMin(aabb2.min), aabb1.max.cwiseMax(aabb2.max));
// }
