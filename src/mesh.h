
#pragma once

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Sparse>

#include "aabb.h"

struct BVHNode;

class Mesh {
public:
  void set(const Eigen::MatrixXf &V_, const Eigen::MatrixXi &F_);
  void setColor(int id, const Eigen::Vector4f &color);
  void computeNormals();
  void computeWeightMatrix();
  void computeAdjacencyMatrix();
  void computeBoundingVolumeHierarchy();
  void computeBoundingBox();
  void constructNodes(int nodeId, int *indices, int *startId, int *endId);

  Eigen::MatrixXf V;
  Eigen::MatrixXi F;
  Eigen::MatrixXf Nf;
  Eigen::MatrixXf N;
  Eigen::MatrixXf C;

  Eigen::SparseMatrix<float> W;
  Eigen::SparseMatrix<int> A;
  std::vector<std::vector<int> > adjList;
  std::vector<BVHNode> nodes;

  AABB boundingBox;
  float surfaceArea;
  Eigen::Vector3f weightedCenter;
};

/* Node for Bounding Volume Hierarcy */
struct BVHNode {
  union {
    struct {
      bool flag;
      int size;
      int startId;
      int endId;
    } leaf;

    struct {
      bool unused;
      int rightChild;
    } inner;
  };
  AABB aabb;
};
