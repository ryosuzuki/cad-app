
#pragma once

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Sparse>

#include "loader.h"
#include "aabb.h"

struct BVHNode;

class Mesh {
public:
  void init(const std::string &filename);
  void setVertexColor(int vid, const Eigen::Vector4f &color);
  void setFaceColor(int fid, const Eigen::Vector4f &color);
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

  Loader loader;
  AABB boundingBox;
  float surfaceArea;
  Eigen::Vector3f weightedCenter;
  Eigen::Vector3f center;
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
