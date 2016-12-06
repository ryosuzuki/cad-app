
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
bool wireframe = false;
bool deformation = false;
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
float mouseDownX;
float mouseDownY;
float mouseDownZ;
Eigen::Quaternionf mouseDownRotation;

int currentVertexId = -1;
int selectVertexId = -1;
int currentFaceId = -1;

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
  gui->addVariable("Wireframe", wireframe);
  gui->addVariable("Deformation", deformation);

  screen->setVisible(true);
  screen->performLayout();

}


void Viewer::load(std::string filename) {
  if (filename.empty()) {
    filename = nanogui::file_dialog({
      {"obj", "Wavefront OBJ"}
    }, false);
  }

  mesh.init(filename);
  ray.init(mesh);
  deform.init(mesh);
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
  if (wireframe) {
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

void Viewer::launch() {
  std::string filename = "../bunny.obj";

  init();
  initCallbacks();

  control.init(viewport);
  shader.init("shader_mesh");
  mesh.init(filename);
  ray.init(mesh);
  deform.init(mesh);
  deform.setConstraint(37, mesh.V.col(37));

  while (glfwWindowShouldClose(window) == 0) {
    glfwPollEvents();
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int frameWidth, frameHeight;
    glfwGetFramebufferSize(window, &frameWidth, &frameHeight);
    glViewport(0, 0, frameWidth, frameHeight);

    if (selectVertexId != -1) {
      mesh.setColor(selectVertexId, selectColor);
    }
    mesh.setColor(37, Eigen::Vector4f(1.0, 0.0, 0.0, 1.0));

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

  glfwSetCharCallback(window, [](GLFWwindow *, unsigned int codepoint) {
    screen->charCallbackEvent(codepoint);
  });

  glfwSetDropCallback(window, [](GLFWwindow *, int count, const char **filenames) {
    screen->dropCallbackEvent(count, filenames);
    // load(filenames[0]);
  });

  glfwSetMouseButtonCallback(window, [](GLFWwindow *, int button, int action, int modifiers) {
    screen->mouseButtonCallbackEvent(button, action, modifiers);

    if (action == GLFW_PRESS) {
      mouseDown = true;
      Eigen::Vector3f coord = control.project(center);

      mouseDownX = currentMouseX;
      mouseDownY = currentMouseY;
      mouseDownZ = coord[2];
      mouseDownRotation = control.getRotation();

      if (selectVertexId != -1) {
        float zval = control.project(mesh.weightedCenter).z();
        Eigen::Vector3f mouse = { currentMouseX, height - currentMouseY, zval };
        Eigen::Vector3f position = control.unproject(mouse);
        Eigen::Vector3f vertex = mesh.V.col(selectVertexId);

        float distance = (vertex - position).norm() / (mesh.boundingBox.max - mesh.boundingBox.min).norm();
        std::cout << distance << std::endl;
      }

    } else {
      mouseDown = false;
    }

    if (currentVertexId != -1) {
      selectVertexId = currentVertexId;
    }

  });

  glfwSetCursorPosCallback(window, [](GLFWwindow *, double x, double y) {
    screen->cursorPosCallbackEvent(x, y);

    float mouseX = x;
    float mouseY = y;

    currentMouseX = x;
    currentMouseY = y;

    double speed = 2.0;

    if (mouseDown) {
      if (deformation) {
        if (selectVertexId == -1) {
          return;
        }
        float zval = control.project(mesh.weightedCenter).z();
        Eigen::Vector3f mouse = { currentMouseX, height - currentMouseY, zval };
        Eigen::Vector3f position = control.unproject(mouse);

        deform.setConstraint(selectVertexId, position);
        deform.solve(mesh.V);
      } else {
        control.updateRotation(mouseX, mouseY, mouseDownX, mouseDownY, speed, mouseDownRotation);
      }

    }

    Eigen::Vector3f p0 = { currentMouseX, height - currentMouseY, 0.0f };
    Eigen::Vector3f p1 = { currentMouseX, height - currentMouseY, 1.0f };
    Eigen::Vector3f pos0 = control.unproject(p0);
    Eigen::Vector3f pos1 = control.unproject(p1);
    ray.set(pos0, (pos1 - pos0).normalized());

    float minTime = std::numeric_limits<float>::infinity();
    Eigen::Vector2f uv;

    if (selectVertexId == -1) {
      for (int i = 0; i < mesh.F.cols(); ++i) {
        float time;
        bool hit = ray.intersectFace(i, time, uv);
        if (hit && time < minTime) {
          currentFaceId = i;
          minTime = time;
          // std::cout << uv << std::endl;
        }
        if (i == mesh.F.cols()) {
          currentFaceId = -1;
        }
        mesh.setColor(mesh.F(0, i), unselectColor);
        mesh.setColor(mesh.F(1, i), unselectColor);
        mesh.setColor(mesh.F(2, i), unselectColor);
      }
    }

    if (currentFaceId != -1 && selectVertexId == -1) {
      currentVertexId = mesh.F(0, currentFaceId);
      mesh.setColor(currentVertexId, selectColor);
      // mesh.setColor(mesh.F(1, currentFaceId), selectColor);
      // mesh.setColor(mesh.F(2, currentFaceId), selectColor);
    }


  });

  glfwSetScrollCallback(window, [](GLFWwindow *, double x, double y) {
    screen->scrollCallbackEvent(x, y);
    control.zoom(y);
  });

  glfwSetFramebufferSizeCallback(window, [](GLFWwindow *, int width, int height) {
    screen->resizeCallbackEvent(width, height);
  });

}

