
#pragma once

#include <iostream>
#include <Eigen/Core>

#include "mesh.h"

class Ray {
public:

  void init(const Mesh &mesh);
  void set(const Eigen::Vector3f &origin_, const Eigen::Vector3f &direction_);
  bool intersect(int &faceId, float &t, Eigen::Vector2f *uv);
  bool intersectAABB(const AABB &aabb);
  bool intersectFace(int faceId, float &t, Eigen::Vector2f &uv);

  Eigen::Vector3f origin;
  Eigen::Vector3f direction;

  float minTime;
  float maxTime;

  Eigen::MatrixXf V;
  Eigen::MatrixXi F;
  Eigen::MatrixXf N;
  std::vector<BVHNode> nodes;
};
