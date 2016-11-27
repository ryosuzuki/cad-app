
#pragma once

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <nanogui/nanogui.h>
#include <igl/readOBJ.h>
#include <igl/two_axis_valuator_fixed_up.h>
#include <igl/quat_to_mat.h>

#include <OpenGL/gl3.h>
// #include <GLFW/glfw3.h>

#include "mesh.h"
#include "opengl.h"

class Viewer {
public:
  void init();
  void launch();

  void setCallbacks();
  void load();
  void save();

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

