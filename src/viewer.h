
#pragma once

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <nanogui/nanogui.h>
#include <igl/readOBJ.h>
#include <igl/two_axis_valuator_fixed_up.h>
#include <igl/quat_to_mat.h>
#include <nanogui/glutil.h>

#include "mesh.h"
#include "opengl.h"
#include "control.h"

class Viewer {
public:
  void init();
  void launch();

  void load();
  void save();
  void setCallbacks();
  void computeCameraMatries();

  GLFWwindow *window;
  // nanogui::Screen *screen;
  Mesh mesh;
  OpenGL opengl;

  Eigen::MatrixXd vertices;
  Eigen::MatrixXi faces;
  Eigen::MatrixXd vertexUvs;
  Eigen::MatrixXi faceUvs;
  Eigen::MatrixXd cornerNormals;
  Eigen::MatrixXi faceNormalIndices;

};

