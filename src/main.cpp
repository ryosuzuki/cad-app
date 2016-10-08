

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <nanogui/nanogui.h>

using nanogui::Alignment;
using nanogui::Arcball;
using nanogui::BoxLayout;
using nanogui::Button;
using nanogui::CheckBox;
using nanogui::Color;
using nanogui::ComboBox;
using nanogui::FormHelper;
using nanogui::GLFramebuffer;
using nanogui::GroupLayout;
using nanogui::ImagePanel;
using nanogui::Label;
using nanogui::MessageDialog;
using nanogui::Orientation;
using nanogui::Popup;
using nanogui::PopupButton;
using nanogui::ProgressBar;
using nanogui::Screen;
using nanogui::Slider;
using nanogui::TextBox;
using nanogui::ToolButton;
using nanogui::VScrollPanel;
using nanogui::Widget;
using nanogui::Window;
using nanogui::frustum;
using nanogui::lookAt;
using nanogui::project;
using nanogui::scale;
using nanogui::translate;
using nanogui::unproject;
using nanogui::utf8;


class Viewer : public Screen {
public:
  Viewer() : Screen(Eigen:: Vector2i(1024, 768), "CAD App") {
    Window *window = new Window(this, "Viewer");
    window->setPosition(Eigen::Vector2i(10, 10));
    window->setLayout(new GroupLayout());

    Eigen::MatrixXf indices(3, 2);
    indices.col(0) << 0, 1, 2;
    indices.col(1) << 2, 3, 0;

    Eigen::MatrixXf positions(3, 4);
    positions.col(0) << -1, -1, 0;
    positions.col(1) <<  1, -1, 0;
    positions.col(2) <<  1,  1, 0;
    positions.col(3) << -1,  1, 0;


    mShader.init(
      /* An identifying name */
      "a_simple_shader",

      /* Vertex shader */
      "#version 330\n"
      "uniform mat4 modelViewProj;\n"
      "in vec3 position;\n"
      "void main() {\n"
      "    gl_Position = modelViewProj * vec4(position, 1.0);\n"
      "}",

      /* Fragment shader */
      "#version 330\n"
      "out vec4 color;\n"
      "uniform float intensity;\n"
      "void main() {\n"
      "    color = vec4(vec3(intensity), 1.0);\n"
      "}"
      );
    mShader.bind();
    mShader.uploadIndices(indices);
    mShader.uploadAttrib("position", positions);
    mShader.setUniform("intensity", 0.5f);
  }

  ~Viewer() {
    mShader.free();
  }

private:
  nanogui::GLShader mShader;
};

int main() {
  std::cout << "Hello World";

  nanogui::init();
  {

    nanogui::ref<Viewer> viewer = new Viewer();
    viewer->drawAll();
    viewer->setVisible(true);

    nanogui::mainloop();
  }

  nanogui::shutdown();
  return 0;
}