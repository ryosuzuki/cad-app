
#pragma once

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <nanogui/nanogui.h>
#include <nanogui/opengl.h>

#include "mesh.h"
#include "shader.h"
#include "control.h"
#include "loader.h"

class Viewer {
public:
  void init();
  void launch();

  void load();
  void save();
  void debug();
  void setCallbacks();
  void computeCameraMatries();

  GLFWwindow *window;
  Mesh mesh;
  GLShader shaderMesh;
  GLShader shaderWireframe;

};

