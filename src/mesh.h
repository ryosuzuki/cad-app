
#pragma once

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>

#include <igl/per_face_normals.h>
#include <igl/per_vertex_normals.h>


class Mesh {
public:
  void set(const Eigen::MatrixXd &V, const Eigen::MatrixXi &F);
  void setUv(const Eigen::MatrixXd &V_UV, const Eigen::MatrixXi &F_UV);
  void computeFaceNormals(const Eigen::MatrixXd &V, const Eigen::MatrixXi &F, Eigen::MatrixXd &N);
  void computeVertexNormals(const Eigen::MatrixXd &V, const Eigen::MatrixXi &F, Eigen::MatrixXd &N);

  Eigen::MatrixXd vertices;
  Eigen::MatrixXi faces;
  Eigen::MatrixXd faceNormals;
  Eigen::MatrixXd vertexNormals;
  Eigen::MatrixXd faceUvs;
  Eigen::MatrixXi vertexUvs;

};

