
#pragma once

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Sparse>

class Mesh {
public:
  void set(const Eigen::MatrixXf &V_, const Eigen::MatrixXi &F_);
  void setColor(int id, const Eigen::Vector4f &color);
  void computeNormals();
  void computeWeightMatrix();
  void computeAdjacencyMatrix();

  Eigen::MatrixXf V;
  Eigen::MatrixXi F;
  Eigen::MatrixXf Nf;
  Eigen::MatrixXf N;
  Eigen::MatrixXf C;

  Eigen::SparseMatrix<float> W;
  Eigen::SparseMatrix<int> A;
  std::vector<std::vector<int> > adjList;

};

