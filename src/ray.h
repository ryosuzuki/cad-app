
#pragma once

#include <iostream>
#include <Eigen/Core>

#include "mesh.h"
#include "control.h"

class Ray {
public:

  void init(const Mesh &mesh, const Eigen::Vector4f &viewport);
  void set(const Eigen::Vector3f &origin_, const Eigen::Vector3f &direction_);
  void updateMesh(const Mesh &mesh);
  void updateViewport(Eigen::Vector4f viewport);
  void setFromMouse(float x, float y, Control control);
  int intersect();
  bool intersect(int &faceId, float &t, Eigen::Vector2f *uv);
  bool intersectAABB(const AABB &aabb);
  bool intersectFace(int faceId, float &t, Eigen::Vector2f &uv);

  Eigen::Vector3f origin;
  Eigen::Vector3f direction;

  float width;
  float height;
  float minTime;
  float maxTime;

  Eigen::MatrixXf V;
  Eigen::MatrixXi F;
  Eigen::MatrixXf N;
  std::vector<BVHNode> nodes;
};
