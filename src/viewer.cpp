
#include "viewer.h"

nanogui::Screen *screen = nullptr;
GLFWwindow *window;
Control control;
Loader loader;

float width = 800;
float height = 800;

bool mouseDown = false;
bool orthographic = false;
bool wireframe = false;
float cameraViewAngle = 45.0;
float cameraNear = 1.0;
float cameraFar = 100.0;
float lineWidth = 0.5f;
float cameraZoom = 3.0f;
float modelZoom = 3.0f;

Eigen::Matrix4f viewMatrix = Eigen::Matrix4f::Identity();
Eigen::Matrix4f projMatrix = Eigen::Matrix4f::Identity();
Eigen::Matrix4f modelMatrix = Eigen::Matrix4f::Identity();
Eigen::Quaternionf arcballQuat = Eigen::Quaternionf::Identity();

Eigen::Vector3f center(0, 1, 0);
Eigen::Vector4f lineColor(1.0f, 1.0f, 1.0f, 1.0f);
Eigen::Vector3f cameraEye(0, 0, 5);
Eigen::Vector3f cameraCenter(0, 1, 0);
Eigen::Vector3f cameraUp(0, 1, 0);
Eigen::Vector3f modelTranslation(0, 0, 0);
Eigen::Vector4f viewport(0, 0, 800, 800);



Eigen::Vector3f lightPosition(0.0f, 0.30f, 5.0f);
Eigen::Vector4f civ = (viewMatrix * modelMatrix).inverse() * Eigen::Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
Eigen::Vector3f cameraLocal = Eigen::Vector3f(civ.head(3));
Eigen::Vector3f baseColor(0.4f, 0.4f, 0.4f);
Eigen::Vector3f specularColor(1.0f, 1.0f, 1.0f);


float currentMouseX;
float currentMouseY;
float mouseDownX;
float mouseDownY;
float mouseDownZ;
Eigen::Quaternionf mouseDownRotation;



void Viewer::init() {

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

  window = glfwCreateWindow(width, height, "CAD app", nullptr, nullptr);
  glfwMakeContextCurrent(window);

  glViewport(0, 0, width, height);
  glfwSwapInterval(0);
  glfwSwapBuffers(window);

  int windowWidth, windowHeight;
  glfwGetWindowSize(window, &windowWidth, &windowHeight);

  screen = new nanogui::Screen();
  screen->initialize(window, true);

  nanogui::FormHelper *gui = new nanogui::FormHelper(screen);
  nanogui::ref<nanogui::Window> guiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Main Menu");
  gui->addGroup("Workspace");
  gui->addButton("Load", [&]() { load(); });
  // gui->addButton("Save", [&]() { this->save(); });
  gui->addVariable("Wireframe", wireframe);

  screen->setVisible(true);
  screen->performLayout();

}

void Viewer::initShaders() {
  std::cout << "Compiling shaders .. ";
  std::cout.flush();
  shaderMesh.initFromFiles("shader_mesh",
                           "../src/shader_mesh.vert",
                           "../src/shader_mesh.frag",
                           "../src/shader_mesh.geom");
  shaderWireframe.initFromFiles("shader_wireframe",
                                "../src/shader_wireframe.vert",
                                "../src/shader_wireframe.frag");
  std::cout << "done." << std::endl;
}

void Viewer::load(std::string filename) {
  std::cout << "Loading OBJ file .. " << std::endl;

  if (filename.empty()) {
    filename = nanogui::file_dialog({
      {"obj", "Wavefront OBJ"}
    }, false);
  }

  Eigen::MatrixXf V;
  Eigen::MatrixXi F;
  loader.loadObj(filename, V, F);
  mesh.set(V, F);

  std::cout << filename << std::endl;
  std::cout << "done." << std::endl;
}

void Viewer::drawMesh() {
  Eigen::Vector4f civ = (viewMatrix * modelMatrix).inverse() * Eigen::Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
  cameraLocal = Eigen::Vector3f(civ.head(3));

  shaderMesh.bind();
  shaderMesh.uploadIndices(mesh.F);
  shaderMesh.uploadAttrib("position", mesh.V);
  shaderMesh.uploadAttrib("normal", mesh.N);
  shaderMesh.setUniform("model", modelMatrix);
  shaderMesh.setUniform("view", viewMatrix);
  shaderMesh.setUniform("proj", projMatrix);
  shaderMesh.setUniform("light_position", lightPosition);
  shaderMesh.setUniform("camera_local", cameraLocal);
  shaderMesh.setUniform("show_uvs", 0.0f);
  shaderMesh.setUniform("base_color", baseColor);
  shaderMesh.setUniform("specular_color", specularColor);

  glDepthFunc(GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);
  if (wireframe) {
    Eigen::Vector4f lineColor = Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(lineWidth);
    shaderMesh.setUniform("fixed_color", lineColor);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);
  }
  shaderMesh.drawIndexed(GL_TRIANGLES, 0, mesh.F.cols());
  shaderMesh.setUniform("fixed_color", Eigen::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
  glDisable(GL_POLYGON_OFFSET_FILL);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Viewer::drawWireframe() {
  if (!wireframe) return;
  shaderWireframe.bind();
  shaderWireframe.uploadAttrib("position", mesh.V);

  glEnable(GL_LINE_SMOOTH);
  glLineWidth(1.0f);
  shaderWireframe.bind();
  shaderWireframe.uploadAttrib("position", mesh.V);
  shaderWireframe.setUniform("color", baseColor);
  shaderWireframe.setUniform("mvp", Eigen::Matrix4f(projMatrix * viewMatrix * modelMatrix));
  shaderWireframe.drawIndexed(GL_LINES, 0, mesh.F.cols());
}


void Viewer::launch() {
  init();
  initShaders();
  initCallbacks();
  load("../bunny.obj");

  while (glfwWindowShouldClose(window) == 0) {
    glfwPollEvents();
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float ratio;
    int frameWidth, frameHeight;
    glfwGetFramebufferSize(window, &frameWidth, &frameHeight);
    ratio = frameWidth / (float)frameHeight;
    glViewport(0, 0, frameWidth, frameHeight);

    computeCameraMatries();
    drawMesh();

    screen->drawContents();
    screen->drawWidgets();

    glfwSwapBuffers(window);
  }

  glfwTerminate();
}


void Viewer::save() {
  std::cout << "save" << std::endl;
}

void Viewer::computeCameraMatries() {

  modelMatrix = Eigen::Matrix4f::Identity();
  viewMatrix = Eigen::Matrix4f::Identity();
  projMatrix = Eigen::Matrix4f::Identity();

  /* Set view parameters */
  viewMatrix = control.lookAt(cameraEye, cameraCenter, cameraUp);

  /* Set projection paramters */
  if (orthographic) {
    float length = (cameraEye - cameraCenter).norm();
    float h = tan(cameraViewAngle / 360.0 * M_PI) * (length);
    float left = -h * width / height;
    float right = h * width / height;
    float bottom = -h;
    float top = h;
    projMatrix = control.ortho(left, right, bottom, top, cameraNear, cameraFar);
  } else {
    float fH = tan(cameraViewAngle / 360.0 * M_PI) * cameraNear;
    float fW = fH * width / height;
    float left = -fW;
    float right = fW;
    float bottom = -fH;
    float top = fH;
    projMatrix = control.frustum(left, right, bottom, top, cameraNear, cameraFar);
  }

  /* Set model parameters */
  modelMatrix = control.quatToMatrix(arcballQuat);
  modelMatrix.topLeftCorner(3, 3) *= cameraZoom;
  modelMatrix.topLeftCorner(3, 3) *= modelZoom;
  modelMatrix.col(3).head(3) += modelMatrix.topLeftCorner(3, 3) * modelTranslation;

  /*
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

    shaderMesh.bind();

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);
    shaderMesh.drawIndexed(GL_TRIANGLES, 0, mesh.F.cols());
    glDisable(GL_POLYGON_OFFSET_FILL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  */

  glfwPostEmptyEvent();
}

void Viewer::initCallbacks() {
  glfwSetKeyCallback(window, [](GLFWwindow *, int key, int scancode, int action, int mods) {
    screen->keyCallbackEvent(key, scancode, action, mods);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

  });

  glfwSetCharCallback(window, [](GLFWwindow *, unsigned int codepoint) {
    screen->charCallbackEvent(codepoint);
  });

  glfwSetDropCallback(window, [](GLFWwindow *, int count, const char **filenames) {
    screen->dropCallbackEvent(count, filenames);
  });

  glfwSetMouseButtonCallback(window, [](GLFWwindow *, int button, int action, int modifiers) {
    screen->mouseButtonCallbackEvent(button, action, modifiers);

    if (action == GLFW_PRESS) {
      mouseDown = true;
      Eigen::Vector3f coord = control.project(center, modelMatrix, projMatrix, viewMatrix, viewport);

      mouseDownX = currentMouseX;
      mouseDownY = currentMouseY;
      mouseDownZ = coord[2];
      mouseDownRotation = arcballQuat.normalized();
    } else {
      mouseDown = false;
    }
    // mouseMode = "ROTATION";

  });

  glfwSetCursorPosCallback(window, [](GLFWwindow *, double x, double y) {
    screen->cursorPosCallbackEvent(x, y);

    float mouseX = x;
    float mouseY = y;

    currentMouseX = x;
    currentMouseY = y;

    double speed = 2.0;
    if (mouseDown) {
      Eigen::Quaternionf diffRotation = control.motion(width, height, mouseX, mouseY, mouseDownX, mouseDownY, speed);
      arcballQuat = diffRotation * mouseDownRotation;
    }

  });

  glfwSetScrollCallback(window, [](GLFWwindow *, double x, double y) {
    screen->scrollCallbackEvent(x, y);

    float deltaY = y;
    if (deltaY != 0) {
      float mult = (1.0 + ((deltaY > 0) ? 1.0 : -1.0) * 0.05);
      const float minZoom = 0.1f;
      cameraZoom = (cameraZoom * mult > minZoom ? cameraZoom * mult : minZoom);
    }

  });

  glfwSetFramebufferSizeCallback(window, [](GLFWwindow *, int width, int height) {
    screen->resizeCallbackEvent(width, height);
  });

}



