
#pragma once

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>

class Mesh {
public:
  void set(const Eigen::MatrixXf &V, const Eigen::MatrixXi &F);
  void setUv(const Eigen::MatrixXf &V_UV, const Eigen::MatrixXi &F_UV);
  void computeFaceNormals(const Eigen::MatrixXf &V, const Eigen::MatrixXi &F, Eigen::MatrixXf &N);
  void computeVertexNormals(const Eigen::MatrixXf &V, const Eigen::MatrixXi &F, Eigen::MatrixXf &N);

  Eigen::MatrixXf vertices;
  Eigen::MatrixXi faces;
  Eigen::MatrixXf faceNormals;
  Eigen::MatrixXf vertexNormals;
  Eigen::MatrixXf faceUvs;
  Eigen::MatrixXi vertexUvs;

  Eigen::MatrixXf faceAmbient;
  Eigen::MatrixXf faceDiffuse;
  Eigen::MatrixXf faceSpecular;

  Eigen::MatrixXf vertexAmbient;
  Eigen::MatrixXf vertexDiffuse;
  Eigen::MatrixXf vertexSpecular;

};

