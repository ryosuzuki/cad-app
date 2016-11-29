
#include "viewer.h"

using namespace nanogui;

nanogui::Screen *screen = nullptr;
Control control;

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

Eigen::Vector3f center(0, 0, 0);
Eigen::Vector4f lineColor(1.0f, 1.0f, 1.0f, 1.0f);
Eigen::Vector3f cameraEye(0, 0, 5);
Eigen::Vector3f cameraCenter(0, 0, 0);
Eigen::Vector3f cameraUp(0, 1, 0);
Eigen::Vector3f modelTranslation(0, 0, 0);
Eigen::Vector4f viewport(0, 0, 800, 800);


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

  // glEnable(GL_DEPTH_TEST);
  // glEnable(GL_LIGHTING);

  window = glfwCreateWindow(width, height, "CAD app", nullptr, nullptr);
  glfwMakeContextCurrent(window);

  glViewport(0, 0, width, height);
  glfwSwapInterval(0);
  glfwSwapBuffers(window);

  int windowWidth, windowHeight;
  glfwGetWindowSize(window, &windowWidth, &windowHeight);

  screen = new Screen();
  screen->initialize(window, true);

  FormHelper *gui = new FormHelper(screen);
  nanogui::ref<Window> guiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Main Menu");
  gui->addGroup("Workspace");
  gui->addButton("Load", [&]() { this->load(); });
  // gui->addButton("Save", [&]() { this->save(); });
  gui->addVariable("Wireframe", wireframe);


  screen->setVisible(true);
  screen->performLayout();


}

void Viewer::load() {
  std::string filename = nanogui::file_dialog({
    {"obj", "Wavefront OBJ"}
    }, false);
  std::cout << filename << std::endl;

  if (filename.empty()) {
    return;
  }

  igl::readOBJ(filename, vertices, vertexUvs, cornerNormals, faces, faceUvs, faceNormalIndices);
  mesh.set(vertices, faces);
  // this->mesh.setUv(vertexUvs, faceUvs);

  // core.align_camera_center(data.V,data.F);
}

void Viewer::save() {
  std::cout << "save" << std::endl;
}


void Viewer::computeCameraMatries() {

  modelMatrix = Eigen::Matrix4f::Identity();
  viewMatrix = Eigen::Matrix4f::Identity();
  projMatrix = Eigen::Matrix4f::Identity();

  // Set view parameters
  viewMatrix = control.lookAt(cameraEye, cameraCenter, cameraUp);

  // Set projection paramters
  if (orthographic) {
    float length = (cameraEye - cameraCenter).norm();
    float h = tan(cameraViewAngle/360.0 * M_PI) * (length);
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

  // Set model parameters
  modelMatrix = control.quatToMatrix(arcballQuat);
  modelMatrix.topLeftCorner(3,3) *= cameraZoom;
  modelMatrix.topLeftCorner(3,3) *= modelZoom;
  modelMatrix.col(3).head(3) += modelMatrix.topLeftCorner(3,3)*modelTranslation;

}

void Viewer::launch() {
  init();
  setCallbacks();

  igl::readOBJ("../bunny.obj", vertices, vertexUvs, cornerNormals, faces, faceUvs, faceNormalIndices);
  mesh.set(vertices, faces);

  opengl.init();

  while (glfwWindowShouldClose(window) == 0 && glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS) {

    glfwPollEvents();
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    float ratio;
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / height;
    glViewport(0, 0, width, height);

    opengl.setMesh(mesh);
    opengl.bindMesh();

    // Set transformaition parameters

    computeCameraMatries();

    // Send transformaition parameters
    GLint model = opengl.shaderMesh.uniform("model");
    GLint view = opengl.shaderMesh.uniform("view");
    GLint proj = opengl.shaderMesh.uniform("proj");
    glUniformMatrix4fv(model, 1, GL_FALSE, modelMatrix.data());
    glUniformMatrix4fv(view, 1, GL_FALSE, viewMatrix.data());
    glUniformMatrix4fv(proj, 1, GL_FALSE, projMatrix.data());

    // Set light parameters
    float shininess = 35.0f;
    float lightOn = 1.0f;
    Eigen::Vector3f lightPosition(0.0f, -0.30f, -5.0f);

    // Send light parameters
    GLint specularExponent = opengl.shaderMesh.uniform("specular_exponent");
    GLint lightPositionWorld = opengl.shaderMesh.uniform("light_position_world");
    GLint lightingFactor = opengl.shaderMesh.uniform("lighting_factor");
    GLint fixedColor = opengl.shaderMesh.uniform("fixed_color");
    GLint textureFactor = opengl.shaderMesh.uniform("texture_factor");

    glUniform1f(specularExponent, shininess);
    Eigen::Vector3f revLight = -1.*lightPosition;
    glUniform3fv(lightPositionWorld, 1, revLight.data());
    glUniform1f(lightingFactor, lightOn);
    glUniform4f(fixedColor, 1.0, 1.0, 1.0, 1.0);

    // Send texture paramters
    if (wireframe) {
      glEnable(GL_LINE_SMOOTH);
      glLineWidth(lineWidth);
      glUniform4f(fixedColor, lineColor[0], lineColor[1],
        lineColor[2], 1.0f);
      opengl.drawMesh(false);
      glUniform4f(fixedColor, 0.0f, 0.0f, 0.0f, 0.0f);
    } else {
      glUniform1f(textureFactor, 1.0f);
      opengl.drawMesh(true);
      glUniform1f(textureFactor, 0.0f);
    }

    screen->drawContents();
    screen->drawWidgets();

    glfwSwapBuffers(window);
  }

  glfwTerminate();
}


void Viewer::setCallbacks() {
  glfwSetKeyCallback(window, [](GLFWwindow *, int key, int scancode, int action, int mods) {
    screen->keyCallbackEvent(key, scancode, action, mods);
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
      mouseDownRotation = arcballQuat;
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
      std::cout << Eigen::Vector4f(mouseDownX, mouseDownY, mouseX, mouseY) << std::endl;
      Eigen::Quaternionf diffRotation = control.motion(width, height, mouseDownX, mouseDownY, mouseX, mouseY, speed);
      arcballQuat = (diffRotation * mouseDownRotation).normalized();
    }

  });

  glfwSetScrollCallback(window, [](GLFWwindow *, double x, double y) {
    screen->scrollCallbackEvent(x, y);

    float deltaY = y;
    std::cout << deltaY << std::endl;
    if (deltaY != 0) {
      float mult = (1.0 + ((deltaY>0) ? 1.0 : -1.0) * 0.05);
      const float minZoom = 0.1f;
      cameraZoom = (cameraZoom * mult > minZoom ? cameraZoom * mult : minZoom);
    }

  });

  glfwSetFramebufferSizeCallback(window, [](GLFWwindow *, int width, int height) {
    screen->resizeCallbackEvent(width, height);
  });

}



