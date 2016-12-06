
#pragma once

#include <iostream>
#include <unordered_map>
#include <Eigen/Core>
#include <Eigen/Sparse>
#include <Eigen/SVD>

#include "mesh.h"

class Deform {
public:

  void set(const Mesh &mesh);
  void solve(Eigen::MatrixXf &U);
  void setConstraint(int id, const Eigen::Vector3f &position);
  void init();
  void initRotations();
  void initConstraints();
  void initLinearSystem();
  void estimateRotations();
  void estimatePositions();

  Eigen::MatrixXf V;
  Eigen::MatrixXi F;
  Eigen::MatrixXf Vprime;
  Eigen::Matrix<float, 3, Eigen::Dynamic> b;
  Eigen::Matrix<float, 3, Eigen::Dynamic> bFixed;

  int freeIdx;
  std::vector<int> freeIdxMap;
  std::vector<Eigen::MatrixXf> R;
  Eigen::SparseMatrix<float> W;
  Eigen::SparseMatrix<float> L;

  std::unordered_map<int, Eigen::Vector3f> constraints;
  Eigen::SimplicialLDLT<Eigen::SparseMatrix<float> > solver;

};
