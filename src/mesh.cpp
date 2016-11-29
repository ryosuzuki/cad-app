

#include "mesh.h"

void Mesh::set(const Eigen::MatrixXd &vertices, const Eigen::MatrixXi &faces) {
  this->vertices = vertices;
  this->faces = faces;
  // this->computeFaceNormals(vertices, faces, this->faceNormals);
  // this->computeVertexNormals(vertices, faces, this->vertexNormals);
}

void Mesh::computeFaceNormals(const Eigen::MatrixXd &V, const Eigen::MatrixXi &F, Eigen::MatrixXd &N) {

  std::cout << "Computing face normals .. " << std::endl;
  std::cout << F.rows() << std::endl;

  N.resize(F.rows(), 3);

  for(int f=0; f<F.rows(); f++) {
    const Eigen::Vector3d p0 = V.row(F(f,0));
    const Eigen::Vector3d p1 = V.row(F(f,1));
    const Eigen::Vector3d p2 = V.row(F(f,2));
    const Eigen::Vector3d n0 = (p1 - p0).cross(p2 - p0);
    const Eigen::Vector3d n1 = (p2 - p1).cross(p0 - p1);
    const Eigen::Vector3d n2 = (p0 - p2).cross(p1 - p2);

    for(int d=0; d<3; d++) {
      N(f,d) = n0(d) + n1(d) + n2(d);
    }
    N.row(f) /= N.row(f).norm();
  }
  // std::cout << N << std::endl;
}

void Mesh::computeVertexNormals(const Eigen::MatrixXd &V, const Eigen::MatrixXi &F, Eigen::MatrixXd &N) {

  std::cout << "Computing vertex normals .. " << std::endl;

  N.resize(V.rows(), V.cols());
  N.setZero();

  for (int f = 0; f<F.rows(); f++) {
    Eigen::Vector3d fn = Eigen::Vector3d::Zero();
    for (int i=0; i<3; ++i) {
      Eigen::Vector3d v0 = V.col(F(f, i)),
      v1 = V.col(F(f, (i+1)%3)),
      v2 = V.col(F(f, (i+2)%3)),
      d0 = v1-v0,
      d1 = v2-v0;

      if (i == 0) {
        fn = d0.cross(d1);
        double norm = fn.norm();
        fn /= norm;
      }

      double cos = d0.dot(d1) / std::sqrt(d0.squaredNorm() * d1.squaredNorm());
      double negate = double(cos < 0.0f);
      cos = std::abs(cos);
      double ret = -0.0187293f;
      ret *= cos; ret = ret + 0.0742610f;
      ret *= cos; ret = ret - 0.2121144f;
      ret *= cos; ret = ret + 1.5707288f;
      ret = ret * std::sqrt(1.0f - cos);
      ret = ret - 2.0f * negate * ret;
      double angle = negate * (double) M_PI + ret;

      for (int k=0; k<3; ++k) {
        double coeff = N.coeffRef(k, F(f, i));
        coeff += fn[k]*angle;
        N.coeffRef(k, F(f, i)) = coeff;
      }

    }
  }
  std::cout << "done." << std::endl;
}
