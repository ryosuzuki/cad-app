
#pragma once

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Sparse>

class Mesh {
public:
  void set(const Eigen::MatrixXf &V_, const Eigen::MatrixXi &F_);
  void computeNormals();
  void computeAdjacencyMatrix();
  void computeAdjacencyList();

  Eigen::MatrixXf V;
  Eigen::MatrixXi F;
  Eigen::MatrixXf Nf;
  Eigen::MatrixXf N;

  Eigen::SparseMatrix<int> A;
  std::vector<std::vector<int> > adjList;

};

