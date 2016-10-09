
#pragma once

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <nanogui/nanogui.h>
#include <igl/readOBJ.h>

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
  nanogui::Screen *screen;
  Mesh mesh;
  OpenGL opengl;

};

