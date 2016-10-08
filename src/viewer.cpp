

#include "viewer.h"

void Viewer::init() {

  using namespace nanogui;

  glfwInit();
  glfwSetTime(0);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwWindowHint(GLFW_SAMPLES, 0);
  glfwWindowHint(GLFW_RED_BITS, 8);
  glfwWindowHint(GLFW_GREEN_BITS, 8);
  glfwWindowHint(GLFW_BLUE_BITS, 8);
  glfwWindowHint(GLFW_ALPHA_BITS, 8);
  glfwWindowHint(GLFW_STENCIL_BITS, 8);
  glfwWindowHint(GLFW_DEPTH_BITS, 24);
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

  GLFWwindow *window = glfwCreateWindow(800, 800, "CAD app", nullptr, nullptr);
  glfwMakeContextCurrent(window);

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  glfwSwapInterval(0);
  glfwSwapBuffers(window);

  Screen *screen = new Screen();
  screen->initialize(window, true);

  FormHelper *gui = new FormHelper(screen);
  nanogui::ref<Window> guiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Form helper example");
  gui->addGroup("Workspace");
  gui->addButton("Load", [&]() { this->load(); });
  gui->addButton("Save", [&]() { this->save(); });

  screen->setVisible(true);
  screen->performLayout();

  Eigen::MatrixXd vertices;
  Eigen::MatrixXi faces;
  Eigen::MatrixXd vertexUvs;
  Eigen::MatrixXi faceUvs;
  Eigen::MatrixXd cornerNormals;
  Eigen::MatrixXi faceNormalIndices;

  igl::readOBJ("../bunny.obj", vertices, vertexUvs, cornerNormals, faces, faceUvs, faceNormalIndices);

  this->mesh.set(vertices, faces);
  // this->mesh.setUv(vertexUvs, faceUvs);

}

void Viewer::load() {
  std::cout << "load" << std::endl;


}

void Viewer::save() {
  std::cout << "save" << std::endl;
}

void Viewer::launch() {
  this->init();
}
