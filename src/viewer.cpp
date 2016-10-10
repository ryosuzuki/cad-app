
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

  window = glfwCreateWindow(800, 800, "CAD app", nullptr, nullptr);
  glfwMakeContextCurrent(window);

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  glfwSwapInterval(0);
  glfwSwapBuffers(window);

  int windowWidth, windowHeight;
  glfwGetWindowSize(window, &windowWidth, &windowHeight);

  screen = new Screen();
  screen->initialize(window, true);

  FormHelper *gui = new FormHelper(screen);
  nanogui::ref<Window> guiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Form helper example");
  gui->addGroup("Workspace");
  gui->addButton("Load", [&]() { this->load(); });
  gui->addButton("Save", [&]() { this->save(); });

  screen->setVisible(true);
  screen->performLayout();

}

void Viewer::launch() {
  init();
  load();
  // setCallbacks();

  opengl.init();

  while (!glfwWindowShouldClose(window)) {

    glClearColor(0.2f, 0.25f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    float ratio;
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;
    glViewport(0, 0, width, height);

    opengl.setMesh(mesh);
    opengl.bindMesh();

    Eigen::Matrix4f modelMatrix = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f viewMatrix  = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f projMatrix  = Eigen::Matrix4f::Identity();

    GLint model = opengl.shaderMesh.uniform("model");
    GLint view = opengl.shaderMesh.uniform("view");
    GLint proj = opengl.shaderMesh.uniform("proj");

    glUniformMatrix4fv(model, 1, GL_FALSE, modelMatrix.data());
    glUniformMatrix4fv(view, 1, GL_FALSE, viewMatrix.data());
    glUniformMatrix4fv(proj, 1, GL_FALSE, projMatrix.data());

    GLint specularExponent = opengl.shaderMesh.uniform("specular_exponent");
    GLint lightPositionWorld = opengl.shaderMesh.uniform("light_position_world");
    GLint lightingFactor = opengl.shaderMesh.uniform("lighting_factor");
    GLint fixedColor = opengl.shaderMesh.uniform("fixed_color");
    GLint textureFactor = opengl.shaderMesh.uniform("texture_factor");



    glUniform1f(texture_factori, 1.0f);
    opengl.drawMesh();


    // mvp_location = glGetUniformLocation(program, "MVP");
    // glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);


    glUniform1f(texture_factori, 0.0f);


    screen->drawContents();
    screen->drawWidgets();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
}

void Viewer::setCallbacks() {
  /*
  glfwSetCursorPosCallback(window,
    [](GLFWwindow *, double x, double y) {
      screen->cursorPosCallbackEvent(x, y);
    }
  );

  glfwSetMouseButtonCallback(window,
    [](GLFWwindow *, int button, int action, int modifiers) {
      screen->mouseButtonCallbackEvent(button, action, modifiers);
    }
  );

  glfwSetKeyCallback(window,
    [](GLFWwindow *, int key, int scancode, int action, int mods) {
      screen->keyCallbackEvent(key, scancode, action, mods);
    }
  );

  glfwSetCharCallback(window,
    [](GLFWwindow *, unsigned int codepoint) {
      screen->charCallbackEvent(codepoint);
    }
  );

  glfwSetDropCallback(window,
    [](GLFWwindow *, int count, const char **filenames) {
      screen->dropCallbackEvent(count, filenames);
    }
  );

  glfwSetScrollCallback(window,
    [](GLFWwindow *, double x, double y) {
      screen->scrollCallbackEvent(x, y);
    }
  );

  glfwSetFramebufferSizeCallback(window,
    [](GLFWwindow *, int width, int height) {
      screen->resizeCallbackEvent(width, height);
    }
  );
  */
}


void Viewer::load() {

  Eigen::MatrixXd vertices;
  Eigen::MatrixXi faces;
  Eigen::MatrixXd vertexUvs;
  Eigen::MatrixXi faceUvs;
  Eigen::MatrixXd cornerNormals;
  Eigen::MatrixXi faceNormalIndices;

  igl::readOBJ("../bunny.obj", vertices, vertexUvs, cornerNormals, faces, faceUvs, faceNormalIndices);

  mesh.set(vertices, faces);
  // this->mesh.setUv(vertexUvs, faceUvs);

  // core.align_camera_center(data.V,data.F);

}

void Viewer::save() {
  std::cout << "save" << std::endl;
}















