
#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <nanogui/nanogui.h>
#include <igl/readOBJ.h>

#include "mesh.h"


class Viewer {
public:
  void init();
  void launch();

  void load();
  void save();

  Mesh mesh;

protected:
  nanogui::GLShader shader_;
};

