
#pragma once

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Sparse>

class Mesh {
public:
  void set(const Eigen::MatrixXf &V_, const Eigen::MatrixXi &F_);
  void computeNormals();
  // void computeAdjacencyMatrix();
  // void computeEdgeLength();
  void computeWeightMatrix();
  // void computeLaplacianMatrix();

  Eigen::MatrixXf V;
  Eigen::MatrixXi F;
  Eigen::MatrixXf Nf;
  Eigen::MatrixXf N;

  Eigen::SparseMatrix<float> W;

  // Eigen::MatrixXf E;
  // Eigen::MatrixXf W;

  // Eigen::SparseMatrix<float> L;
  // Eigen::SparseMatrix<int> A;
  // std::vector<std::vector<int> > adjList;

};

