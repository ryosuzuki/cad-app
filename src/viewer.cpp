
#include "viewer.h"

nanogui::Screen *screen = nullptr;
GLFWwindow *window;

Mesh mesh;
Control control;
Ray ray;
Deform deform;
Shader shader;

float width = 800;
float height = 800;

bool mouseDown = false;
bool wireframeMode = false;
bool deformMode = false;
bool constraintMode = false;
float lineWidth = 0.5f;
// std::string mouseMode = "ROTATION";

Eigen::Vector3f center(0, 1, 0);
Eigen::Vector4f lineColor(1.0f, 1.0f, 1.0f, 1.0f);
Eigen::Vector4f viewport(0, 0, 800, 800);

Eigen::Vector3f lightPosition(0.0f, 0.30f, 5.0f);
Eigen::Vector3f baseColor(0.4f, 0.4f, 0.4f);
Eigen::Vector3f specularColor(1.0f, 1.0f, 1.0f);
Eigen::Vector4f selectColor(1.0f, 0.0f, 1.0f, 1.0f);
Eigen::Vector4f unselectColor(0.0f, 0.0f, 0.0f, 0.0f);

Eigen::Matrix4f vertexColor;

float currentMouseX;
float currentMouseY;
float currentMouseZ;
float mouseDownX;
float mouseDownY;
float mouseDownZ;
Eigen::Quaternionf mouseDownRotation;
Eigen::Vector3f mouseDown3D;
Eigen::Vector3f currentMouse3D;


int currentVertexId = -1;
int selectVertexId = -1;
int currentFaceId = -1;
int mouseDownFaceId = -1;

int deformId = -1;

void Viewer::init() {
  using namespace nanogui;
  using namespace std;

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

  Window *widget = new Window(screen, "Main Menu");
  widget->setPosition(Vector2i(15, 15));
  widget->setLayout(new GroupLayout());

  new Label(widget, "Load OBJ file");
  Button *button;
  button = new Button(widget, "Load");
  button->setCallback([&] {
    std::string filename = nanogui::file_dialog({
      {"obj", "Wavefront OBJ"}
    }, false);
    mesh.init(filename);
    ray.init(mesh, viewport);
    deform.init(mesh);
  });

  new Label(widget, "Toggle wireframe");
  CheckBox *checkBox;
  checkBox = new CheckBox(widget, "Wireframe");
  checkBox->setCallback([&] (bool state) {
    wireframeMode = state;
  });

  new Label(widget, "Deformation");
  button = new Button(widget, "Add Constraints");
  button->setFlags(Button::ToggleButton);
  button->setChangeCallback([](bool state) {
    constraintMode = state;
  });

  button = new Button(widget, "Deform");
  button->setFlags(Button::ToggleButton);
  button->setChangeCallback([](bool state) {
    deformMode = state;
  });


  screen->setVisible(true);
  screen->performLayout();
}

void Viewer::launch() {
  std::string filename = "../data/bunny.obj";

  init();
  initCallbacks();

  shader.init("shader_mesh");
  mesh.init(filename);
  deform.init(mesh);
  ray.init(mesh, viewport);
  control.init(viewport);

  while (glfwWindowShouldClose(window) == 0) {
    glfwPollEvents();
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int frameWidth, frameHeight;
    glfwGetFramebufferSize(window, &frameWidth, &frameHeight);
    glViewport(0, 0, frameWidth, frameHeight);

    control.computeCameraMatries();
    drawMesh();

    screen->drawContents();
    screen->drawWidgets();

    glfwSwapBuffers(window);
  }

  glfwTerminate();
}


void Viewer::initCallbacks() {
  glfwSetKeyCallback(window, [](GLFWwindow *, int key, int scancode, int action, int mods) {
    screen->keyCallbackEvent(key, scancode, action, mods);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
  });

  glfwSetMouseButtonCallback(window, [](GLFWwindow *, int button, int action, int modifiers) {
    screen->mouseButtonCallbackEvent(button, action, modifiers);

    if (action != GLFW_PRESS) {
      mouseDown = false;
      mouseDownFaceId = -1;
      deformId = -1;
      return;
    }

    mouseDown = true;
    mouseDownX = currentMouseX;
    mouseDownY = currentMouseY;
    mouseDownZ = control.project(mesh.weightedCenter)[2];
    mouseDownRotation = control.getRotation();
    mouseDown3D = control.unproject(Eigen::Vector3f(mouseDownX, height-mouseDownY, mouseDownZ));

    ray.setFromMouse(mouseDownX, mouseDownY, control);
    mouseDownFaceId = ray.intersect();
    std::cout << mouseDownFaceId << std::endl;

    if (constraintMode && mouseDownFaceId != -1) {
      for (int i = 0; i < 3; ++i) {
        int id = mesh.F(i, currentFaceId);
        deform.setConstraint(id, mesh.V.col(id));
        mesh.setVertexColor(id, selectColor);
      }
    }

    if (deformMode && mouseDownFaceId != -1) {
      deformId = mesh.F(0, currentFaceId);
    } else {
      deformId = -1;
    }


    if (currentVertexId != -1) {
      selectVertexId = currentVertexId;
    }

  });

  glfwSetCursorPosCallback(window, [](GLFWwindow *, double x, double y) {
    screen->cursorPosCallbackEvent(x, y);

    ray.setFromMouse(x, y, control);
    currentFaceId = ray.intersect();
    currentMouseX = x;
    currentMouseY = y;
    currentMouseZ = control.project(mesh.weightedCenter)[2];
    currentMouse3D = control.unproject(Eigen::Vector3f(currentMouseX, height-currentMouseY, currentMouseZ));

    if (mouseDown) {
      if (deformMode && deformId != -1 && mouseDownFaceId != -1) {
        std::cout << currentMouse3D << std::endl;
        deform.setConstraint(deformId, currentMouse3D);
        mesh.setVertexColor(deformId, selectColor);
        deform.solve(mesh.V);
      } else {
        control.updateRotation(currentMouseX, currentMouseY, mouseDownX, mouseDownY, mouseDownRotation);
      }
    }

  });

  glfwSetScrollCallback(window, [](GLFWwindow *, double x, double y) {
    screen->scrollCallbackEvent(x, y);
    control.zoom(y);
  });

  glfwSetCharCallback(window, [](GLFWwindow *, unsigned int codepoint) {
    screen->charCallbackEvent(codepoint);
  });

  glfwSetDropCallback(window, [](GLFWwindow *, int count, const char **filenames) {
    screen->dropCallbackEvent(count, filenames);
    // load(filenames[0]);
  });

  glfwSetFramebufferSizeCallback(window, [](GLFWwindow *, int width, int height) {
    screen->resizeCallbackEvent(width, height);
  });

}


void Viewer::drawMesh() {
  shader.bind();
  shader.uploadIndices(mesh.F);
  shader.uploadAttrib("position", mesh.V);
  shader.uploadAttrib("normal", mesh.N);
  shader.uploadAttrib("color", mesh.C);
  shader.setUniform("model", control.model);
  shader.setUniform("view", control.view);
  shader.setUniform("proj", control.proj);
  shader.setUniform("camera_local", control.cameraLocal);
  shader.setUniform("light_position", lightPosition);
  shader.setUniform("show_uvs", 0.0f);
  shader.setUniform("base_color", baseColor);
  shader.setUniform("specular_color", specularColor);

  glDepthFunc(GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);
  if (wireframeMode) {
    Eigen::Vector4f lineColor = Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(lineWidth);
    shader.setUniform("fixed_color", lineColor);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);
  }
  shader.drawIndexed(GL_TRIANGLES, 0, mesh.F.cols());
  shader.setUniform("fixed_color", Eigen::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
  glDisable(GL_POLYGON_OFFSET_FILL);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

