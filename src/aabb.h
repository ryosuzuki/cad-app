
#pragma once

#include <iostream>
#include <Eigen/Core>

class AABB {
public:

  void clear();
  void expandBy(const Eigen::Vector3f &p);
  bool contains(const Eigen::Vector3f &p);
  float distanceTo(const Eigen::Vector3f &p);
  float surfaceArea();
  Eigen::Vector3f center();

  Eigen::Vector3f min;
  Eigen::Vector3f max;
};


