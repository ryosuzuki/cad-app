

#include "mesh.h"

void Mesh::set(const Eigen::MatrixXf &V_, const Eigen::MatrixXi &F_) {
  V = V_;
  F = F_;
  computeNormals();
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
