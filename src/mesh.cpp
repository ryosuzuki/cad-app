
#include "mesh.h"

void Mesh::init(const std::string &filename) {
  loader.loadObj(filename, V, F);
  C.resize(4, V.cols());
  C.setZero();
  computeNormals();

  /* Pre-computation for mesh deformation */
  computeAdjacencyMatrix();
  computeWeightMatrix();

  /* Pre-computation for fast ray tracing */
  computeBoundingBox();
  computeBoundingVolumeHierarchy();
}

void Mesh::setVertexColor(int vid, const Eigen::Vector4f &color) {
  C.col(vid) = color;
}

void Mesh::setFaceColor(int fid, const Eigen::Vector4f &color) {
  for (int i = 0; i< 3; ++i) {
    int vid = F(i, fid);
    C.col(vid) = color;
  }
}

void Mesh::computeNormals() {
  std::cout << "Computing face and vertex normals .. ";

  Nf.resize(3, F.cols());
  Nf.setZero();

  N.resize(3, V.cols());
  N.setZero();

  for (int f = 0; f < F.cols(); f++) {
    Eigen::Vector3f fn = Eigen::Vector3f::Zero();
    for (int i = 0; i < 3; ++i) {
      Eigen::Vector3f v0 = V.col(F(i, f));
      Eigen::Vector3f v1 = V.col(F((i + 1) % 3, f));
      Eigen::Vector3f v2 = V.col(F((i + 2) % 3, f));
      Eigen::Vector3f d0 = v1 - v0;
      Eigen::Vector3f d1 = v2 - v0;

      if (i == 0) {
        fn = d0.cross(d1);
        float norm = fn.norm();
        fn /= norm;
        Nf.col(f) = fn;
      }

      float cos = d0.dot(d1) / std::sqrt(d0.squaredNorm() * d1.squaredNorm());
      float negate = float(cos < 0.0f);
      cos = std::abs(cos);
      float ret = -0.0187293f;
      ret *= cos; ret = ret + 0.0742610f;
      ret *= cos; ret = ret - 0.2121144f;
      ret *= cos; ret = ret + 1.5707288f;
      ret = ret * std::sqrt(1.0f - cos);
      ret = ret - 2.0f * negate * ret;
      float angle = negate * (float) M_PI + ret;

      for (int k = 0; k < 3; ++k) {
        float coeff = N.coeffRef(k, F(i, f));
        coeff += fn[k] * angle;
        N.coeffRef(k, F(i, f)) = coeff;
      }
    }
  }
  std::cout << "done." << std::endl;
}

void Mesh::computeWeightMatrix() {
  std::cout << "Computing weight matrix.. ";

  typedef Eigen::Triplet<float> T;
  std::vector<T> w_ij;
  w_ij.reserve(F.cols() * 3);

  for (int i = 0; i < F.cols(); i++) {
    int a = F(0, i);
    int b = F(1, i);
    int c = F(2, i);

    Eigen::Vector3f va = V.col(a);
    Eigen::Vector3f vb = V.col(b);
    Eigen::Vector3f vc = V.col(c);

    float la = (vb - vc).squaredNorm();
    float lb = (va - vb).squaredNorm();
    float lc = (vc - va).squaredNorm();

    la = std::sqrt(la);
    lb = std::sqrt(lb);
    lc = std::sqrt(lc);

    // Heron's formula
    float arg =
      (la + (lb + lc)) *
      (lc - (la - lb)) *
      (lc + (la - lb)) *
      (la + (lb - lc));
    float area = 0.25 * sqrt(arg);
    float denom = 1.0 / (4.0 * area);

    // Cotangent weight
    float cot_a = (lb * lb + lc * lc - la * la) * denom;
    float cot_b = (lc * lc + la * la - lb * lb) * denom;
    float cot_c = (la * la + lb * lb - lc * lc) * denom;

    cot_a = std::max<float>(float(1e-10), cot_a);
    cot_b = std::max<float>(float(1e-10), cot_b);
    cot_c = std::max<float>(float(1e-10), cot_c);

    w_ij.push_back(T(a, b, 0.5 * cot_c));
    w_ij.push_back(T(b, a, 0.5 * cot_c));

    w_ij.push_back(T(b, c, 0.5 * cot_a));
    w_ij.push_back(T(c, b, 0.5 * cot_a));

    w_ij.push_back(T(c, a, 0.5 * cot_b));
    w_ij.push_back(T(a, c, 0.5 * cot_b));
  }

  W.resize(V.cols(), V.cols());
  W.reserve(Eigen::VectorXi::Constant(V.cols(), 7));
  W.setZero();
  W.setFromTriplets(w_ij.begin(), w_ij.end());

  std::cout << "done." << std::endl;
}


void Mesh::computeAdjacencyMatrix() {
  std::cout << "Computing adjacency matrix.. ";

  typedef Eigen::Triplet<int> T;
  std::vector<T> adj_ij;
  adj_ij.reserve(F.size() * 2);

  adjList.clear();
  adjList.resize(F.maxCoeff() + 1);

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < F.cols(); j++) {
      int source = F(i, j);
      int dest = F((i + 1) % 3, j);
      adj_ij.push_back(T(source, dest, 1));
      adj_ij.push_back(T(dest, source, 1));

      adjList.at(source).push_back(dest);
      adjList.at(dest).push_back(source);
    }
  }

  A.resize(V.cols(), V.cols());
  A.reserve(6 * (F.maxCoeff() + 1));
  A.setFromTriplets(adj_ij.begin(), adj_ij.end());

  // Set all non-zero to 1
  for (int k = 0; k < A.outerSize(); ++k) {
    for (typename Eigen::SparseMatrix<int>::InnerIterator it (A, k); it; ++it) {
      assert(it.value() != 0);
      A.coeffRef(it.row(), it.col()) = 1;
    }
  }

  // Remove duplicates
  for (int i = 0; i < (int)adjList.size(); i++) {
    std::sort(adjList[i].begin(), adjList[i].end());
    adjList[i].erase(std::unique(adjList[i].begin(), adjList[i].end()), adjList[i].end());
  }

  std::cout << "done." << std::endl;
}


void Mesh::computeBoundingBox() {
  std::cout << "Computing bounding box.. ";

  boundingBox.clear();
  for (int f = 0; f < F.cols(); ++f) {
    Eigen::Vector3f v[3] = { V.col(F(0, f)), V.col(F(1, f)), V.col(F(2, f)) };
    Eigen::Vector3f faceCenter = Eigen::Vector3f::Zero();
    float averageEdgeLength = 0.0;
    float maximumEdgeLength = 0.0;
    for (int i = 0; i < 3; ++i) {
      float edgeLength = (v[i] - v[i == 2 ? 0 : (i + 1)]).norm();
      averageEdgeLength += edgeLength;
      maximumEdgeLength = std::max(maximumEdgeLength, (float) edgeLength);
      boundingBox.expandBy(v[i]);
      faceCenter += v[i];
    }
    faceCenter *= 1.0f / 3.0f;
    float faceArea = 0.5f * (v[1] - v[0]).cross(v[2] - v[0]).norm();
    surfaceArea += faceArea;
    weightedCenter += faceArea * faceCenter;
  }
  std::cout << "done." << std::endl;
  std::cout << boundingBox.min << std::endl;
  std::cout << boundingBox.max << std::endl;
}

void Mesh::computeBoundingVolumeHierarchy() {
  std::cout << "Computing bounding volume hierarchy.. ";

  nodes.resize(2 * F.cols());
  memset(nodes.data(), 0, sizeof(BVHNode) * nodes.size());
  nodes[0].aabb = boundingBox;

  int *indices = new int[F.cols()];
  for (int i = 0; i < F.cols(); ++i) {
    indices[i] = i;
  }
  // constructNodes(0, indices, indices, indices + F.cols());
  std::cout << "done." << std::endl;
}

void Mesh::constructNodes(int nodeId, int *indices, int *startId, int *endId) {
  BVHNode &node = nodes[nodeId];
  int size = endId - startId;
  float bestCost = 1.0 * size;
  int bestIndex = -1;
  int bestAxis = -1;
  std::vector<float> leftAreas(size + 1);

  for (int axis = 0; axis < 3; ++axis) {
    std::sort(startId, endId, [&](int f1, int f2) {
      return
        (V(axis, F(0, f1)) + V(axis, F(1, f1)) + V(axis, F(2, f1))) <
        (V(axis, F(0, f2)) + V(axis, F(1, f2)) + V(axis, F(2, f2)));
    });

    AABB aabb;
    for (int i = 0; i < size; ++i) {
      int f = *(startId + i);
      aabb.expandBy(V.col(F(0, f)));
      aabb.expandBy(V.col(F(1, f)));
      aabb.expandBy(V.col(F(2, f)));
      leftAreas[i] = (float) aabb.surfaceArea();
    }
    if (axis == 0) {
      node.aabb = aabb;
    }
    aabb.clear();

    float aabbFactor = 1.0;
    float triFactor = 1.0 / node.aabb.surfaceArea();
    for (int i = size - 1; i >= 1; --i) {
      int f = *(startId + i);
      aabb.expandBy(V.col(F(0, f)));
      aabb.expandBy(V.col(F(1, f)));
      aabb.expandBy(V.col(F(2, f)));

      float leftArea = leftAreas[i - 1];
      float rightArea = aabb.surfaceArea();
      int primsLeft = i;
      int primsRight = size - i;

      /* Compute SAH cost function */
      float sah_cost = 2.0f * aabbFactor
                       + triFactor * primsLeft * leftArea
                       + triFactor * primsRight * rightArea;
      if (sah_cost < bestCost) {
        bestCost = sah_cost;
        bestIndex = i;
        bestAxis = axis;
      }
    }
  }

  if (bestIndex == -1) {
    /* Splitting does not reduce the cost => make a leaf node */
    node.leaf.flag = true;
    node.leaf.size = size;
    node.leaf.startId = startId - indices;
    node.leaf.endId = node.leaf.startId + size;

    // std::cout << node.leaf.endId << std::endl;

    return;
  }

  /* Splitting based on the best axis */
  std::sort(startId, endId, [&](int f1, int f2) {
    return
      (V(bestAxis, F(0, f1)) + V(bestAxis, F(1, f1)) + V(bestAxis, F(2, f1))) <
      (V(bestAxis, F(0, f2)) + V(bestAxis, F(1, f2)) + V(bestAxis, F(2, f2)));
  });

  int leftCount = bestIndex;
  int leftNodeId = nodeId + 1;
  int rightNodeId = nodeId + 2 * leftCount;
  node.inner.rightChild = rightNodeId;
  node.inner.unused = false;

  constructNodes(leftNodeId, indices, startId, startId + leftCount);
  constructNodes(rightNodeId, indices, startId + leftCount, endId);
};

