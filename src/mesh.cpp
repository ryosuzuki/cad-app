

#include "mesh.h"

void Mesh::set(const Eigen::MatrixXf &V_, const Eigen::MatrixXi &F_) {
  V = V_;
  F = F_;
  computeNormals();
  computeAdjacencyMatrix();
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
      Eigen::Vector3f v1 = V.col(F((i+1)%3, f));
      Eigen::Vector3f v2 = V.col(F((i+2)%3, f));
      Eigen::Vector3f d0 = v1-v0;
      Eigen::Vector3f d1 = v2-v0;

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

void Mesh::computeAdjacencyMatrix() {
  typedef Eigen::Triplet<int> T;
  std::vector<T> adj_ij;
  adj_ij.reserve(F.size()*2);

  for (int i=0; i<3; i++) {
    for (int j=0; j<F.cols(); j++) {
      int source = F(i, j);
      int dest = F((i+1)%3, j);
      adj_ij.push_back(T(source, dest, 1));
      adj_ij.push_back(T(dest, source, 1));
    }
  }

  A.resize(V.cols(), V.cols());
  A.reserve(6*(F.maxCoeff()+1));

  A.setFromTriplets(adj_ij.begin(), adj_ij.end());

  for(int k=0; k<A.outerSize(); ++k) {
    for(typename Eigen::SparseMatrix<int>::InnerIterator it (A,k); it; ++it) {
      assert(it.value() != 0);
      A.coeffRef(it.row(),it.col()) = 1;
    }
  }
  std::cout << A << std::endl;
}

void Mesh::computeAdjacencyList() {
  std::vector<std::vector<int> > adjList;
  adjList.clear();
  adjList.resize(F.maxCoeff()+1);

  for (int i=0; i<3; i++) {
    for (int j=0; j<F.cols(); j++) {
      int source = F(i, j);
      int dest = F((i+1)%3, j);
      adjList.at(source).push_back(dest);
      adjList.at(dest).push_back(source);
    }
  }

  // Remove duplicates
  for (int i=0; i<(int)adjList.size(); i++) {
    std::sort(adjList[i].begin(), adjList[i].end());
    adjList[i].erase(std::unique(adjList[i].begin(), adjList[i].end()), adjList[i].end());
  }
}








