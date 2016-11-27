
#pragma once

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <nanogui/nanogui.h>
#include <igl/readOBJ.h>

#include <OpenGL/gl3.h>
// #include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;


#include "mesh.h"
#include "opengl.h"
#include "controls.h"

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

