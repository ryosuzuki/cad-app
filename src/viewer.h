
#pragma once

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <nanogui/nanogui.h>
#include <nanogui/glutil.h>

#include "mesh.h"
#include "opengl.h"
#include "control.h"
#include "loader.h"

class Viewer {
public:
  void init();
  void launch();

  void load();
  void save();
  void setCallbacks();
  void computeCameraMatries();

  GLFWwindow *window;
  Mesh mesh;
  OpenGL opengl;

  Eigen::MatrixXf vertices;
  Eigen::MatrixXi faces;
  Eigen::MatrixXf vertexUvs;
  Eigen::MatrixXi faceUvs;
  Eigen::MatrixXf cornerNormals;
  Eigen::MatrixXi faceNormalIndices;

};

