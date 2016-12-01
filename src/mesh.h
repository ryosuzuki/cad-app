
#pragma once

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>

class Mesh {
public:
  void set(const Eigen::MatrixXf &V_, const Eigen::MatrixXi &F_);
  void computeNormals();

  Eigen::MatrixXf V;
  Eigen::MatrixXi F;
  Eigen::MatrixXf Nf;
  Eigen::MatrixXf N;

};

