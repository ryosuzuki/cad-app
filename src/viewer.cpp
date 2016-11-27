
#include "viewer.h"

using namespace nanogui;

nanogui::Screen *screen = nullptr;

bool orthographic = false;
bool wireframe = false;
float cameraViewAngle = 45.0;
float cameraNear = 1.0;
float cameraFar = 100.0;
float lineWidth = 0.5f;
float cameraZoom = 3.0f;
float modelZoom = 3.0f;
Eigen::Quaternionf trackballAngle = Eigen::Quaternionf::Identity();
Eigen::Matrix4f viewMatrix = Eigen::Matrix4f::Identity();
Eigen::Matrix4f projMatrix = Eigen::Matrix4f::Identity();
Eigen::Matrix4f modelMatrix = Eigen::Matrix4f::Identity();

Eigen::Vector4f lineColor(0.0f, 0.0f, 0.0f, 1.0f);
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
  // gui->addButton("Save", [&]() { this->save(); });

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
    ratio = width / (float) height;
    glViewport(0, 0, width, height);

    opengl.setMesh(mesh);
    opengl.bindMesh();

    // Set transformaition parameters

    modelMatrix = Eigen::Matrix4f::Identity();
    viewMatrix = Eigen::Matrix4f::Identity();
    projMatrix = Eigen::Matrix4f::Identity();

    // Set view parameters
    Vector3f f = (cameraCenter - cameraEye).normalized();
    Vector3f s = f.cross(cameraUp).normalized();
    Vector3f u = s.cross(f);
    viewMatrix(0,0) = s(0);
    viewMatrix(0,1) = s(1);
    viewMatrix(0,2) = s(2);
    viewMatrix(1,0) = u(0);
    viewMatrix(1,1) = u(1);
    viewMatrix(1,2) = u(2);
    viewMatrix(2,0) =-f(0);
    viewMatrix(2,1) =-f(1);
    viewMatrix(2,2) =-f(2);
    viewMatrix(0,3) =-s.transpose() * cameraEye;
    viewMatrix(1,3) =-u.transpose() * cameraEye;
    viewMatrix(2,3) = f.transpose() * cameraEye;

    // Set projection paramters
    if (orthographic) {
      float length = (cameraEye - cameraCenter).norm();
      float h = tan(cameraViewAngle/360.0 * M_PI) * (length);
      float left = -h*(double)width/(double)height;
      float right = h*(double)width/(double)height;
      float bottom = -h;
      float top = h;
      projMatrix.setIdentity();
      projMatrix(0,0) = 2. / (right - left);
      projMatrix(1,1) = 2. / (top - bottom);
      projMatrix(2,2) = - 2./ (cameraFar - cameraNear);
      projMatrix(0,3) = - (right + left) / (right - left);
      projMatrix(1,3) = - (top + bottom) / (top - bottom);
      projMatrix(2,3) = - (cameraFar + cameraNear) / (cameraFar - cameraNear);
    } else {
      float fH = tan(cameraViewAngle / 360.0 * M_PI) * cameraNear;
      float fW = fH * (double)width/(double)height;
      float left = -fW;
      float right = fW;
      float bottom = -fH;
      float top = fH;
      projMatrix.setConstant(4,4,0.);
      projMatrix(0,0) = (2.0 * cameraNear) / (right - left);
      projMatrix(1,1) = (2.0 * cameraNear) / (top - bottom);
      projMatrix(0,2) = (right + left) / (right - left);
      projMatrix(1,2) = (top + bottom) / (top - bottom);
      projMatrix(2,2) = -(cameraFar + cameraNear) / (cameraFar - cameraNear);
      projMatrix(3,2) = -1.0;
      projMatrix(2,3) = -(2.0 * cameraFar * cameraNear) / (cameraFar - cameraNear);
    }

    // Set model parameters
    float mat[16];
    /*
    float quat[4];
    quat = trackballAngle.coeffs().data();
    float yy2 = 2.0f * quat[1] * quat[1];
    float xy2 = 2.0f * quat[0] * quat[1];
    float xz2 = 2.0f * quat[0] * quat[2];
    float yz2 = 2.0f * quat[1] * quat[2];
    float zz2 = 2.0f * quat[2] * quat[2];
    float wz2 = 2.0f * quat[3] * quat[2];
    float wy2 = 2.0f * quat[3] * quat[1];
    float wx2 = 2.0f * quat[3] * quat[0];
    float xx2 = 2.0f * quat[0] * quat[0];
    mat[0*4+0] = - yy2 - zz2 + 1.0f;
    mat[0*4+1] = xy2 + wz2;
    mat[0*4+2] = xz2 - wy2;
    mat[0*4+3] = 0;
    mat[1*4+0] = xy2 - wz2;
    mat[1*4+1] = - xx2 - zz2 + 1.0f;
    mat[1*4+2] = yz2 + wx2;
    mat[1*4+3] = 0;
    mat[2*4+0] = xz2 + wy2;
    mat[2*4+1] = yz2 - wx2;
    mat[2*4+2] = - xx2 - yy2 + 1.0f;
    mat[2*4+3] = 0;
    mat[3*4+0] = mat[3*4+1] = mat[3*4+2] = 0;
    mat[3*4+3] = 1;
    */
    igl::quat_to_mat(trackballAngle.coeffs().data(), mat);
    for (unsigned i=0;i<4;++i) {
      for (unsigned j=0;j<4;++j) {
        modelMatrix(i, j) = mat[i+4*j];
      }
    }
    modelMatrix.topLeftCorner(3,3) *= cameraZoom;
    modelMatrix.topLeftCorner(3,3) *= modelZoom;
    modelMatrix.col(3).head(3) += modelMatrix.topLeftCorner(3,3)*modelTranslation;

    // Send transformaition parameters
    GLint model = opengl.shaderMesh.uniform("model");
    GLint view = opengl.shaderMesh.uniform("view");
    GLint proj = opengl.shaderMesh.uniform("proj");
    glUniformMatrix4fv(model, 1, GL_FALSE, modelMatrix.data());
    glUniformMatrix4fv(view, 1, GL_FALSE, viewMatrix.data());
    glUniformMatrix4fv(proj, 1, GL_FALSE, projMatrix.data());

    // Set light parameters
    float shininess = 35.0f;
    float factor = 1.0f;
    Eigen::Vector3f lightPosition(0.0f, -0.30f, -5.0f);
    Eigen::Vector3f revLight = -1.*lightPosition;

    // Send light parameters
    GLint specularExponent = opengl.shaderMesh.uniform("specular_exponent");
    GLint lightPositionWorld = opengl.shaderMesh.uniform("light_position_world");
    GLint lightingFactor = opengl.shaderMesh.uniform("lighting_factor");
    glUniform1f(specularExponent, shininess);
    glUniform3fv(lightPositionWorld, 1, revLight.data());
    glUniform1f(lightingFactor, factor);

    // Set texture parameters

    // Send texture paramters
    GLint textureFactor = opengl.shaderMesh.uniform("texture_factor");
    GLint fixedColor = opengl.shaderMesh.uniform("fixed_color");
    if (wireframe) {
      glLineWidth(lineWidth);
      glUniform4f(fixedColor, lineColor[0], lineColor[1],
        lineColor[2], 1.0f);
      opengl.drawMesh(false);
      glUniform4f(fixedColor, 0.0f, 0.0f, 0.0f, 0.0f);
      } else {
        glUniform1f(textureFactor, 1.0f);
        opengl.drawMesh();
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

      Eigen::Vector3f coord;
      Eigen::Vector4f tmp;
      Eigen::Vector3f center(0, 0, 0);
      tmp << center,1;
      tmp = viewMatrix * modelMatrix * tmp;
      tmp = projMatrix * tmp;
      tmp = tmp.array() / tmp(3);
      tmp = tmp.array() * 0.5f + 0.5f;
      tmp(0) = tmp(0) * viewport(2) + viewport(0);
      tmp(1) = tmp(1) * viewport(3) + viewport(1);
      coord = tmp.head(3);

      mouseDownX = currentMouseX;
      mouseDownY = currentMouseY;
      mouseDownZ = coord[2];
      mouseDownRotation = trackballAngle;

    // mouseMode = "ROTATION";

    });

    glfwSetCursorPosCallback(window, [](GLFWwindow *, double x, double y) {
      screen->cursorPosCallbackEvent(x, y);

      float mouseX = x;
      float mouseY = y;

      currentMouseX = x;
      currentMouseY = y;

      std::cout << Eigen::Vector4f(mouseDownX, mouseDownY, mouseX, mouseY) << std::endl;
      std::cout << Eigen::Vector4f(mouseDownX, mouseDownY, mouseX, mouseY) << std::endl;

      float width = viewport(2);
      float height = viewport(3);
      double speed = 2.0;

      if (mouseDownX == 0 || mouseDownY == 0) {
        return;
      }
      igl::two_axis_valuator_fixed_up(
        width,
        height,
        speed,
        mouseDownRotation,
        mouseDownX,
        mouseDownY,
        mouseX,
        mouseY,
        trackballAngle
      );

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



/*
computeMatricesFromInputs();
glm::mat4 ProjectionMatrix = getProjectionMatrix();
glm::mat4 ViewMatrix = getViewMatrix();
glm::mat4 ModelMatrix = glm::mat4(1.0);
glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

// Send our transformation to the currently bound shader,
// in the "MVP" uniform
glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

// Bind our texture in Texture Unit 0
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, Texture);
// Set our "myTextureSampler" sampler to user Texture Unit 0
glUniform1i(TextureID, 0);

// 1rst attribute buffer : vertices
glEnableVertexAttribArray(0);
*/

/*
shininess = 35.0f;

// Default colors
background_color << 0.3f, 0.3f, 0.5f, 1.0f;
line_color << 0.0f, 0.0f, 0.0f, 1.0f;

// Default lights settings
light_position << 0.0f, -0.30f, -5.0f;
lighting_factor = 1.0f; //on

// Default trackball
trackball_angle = Eigen::Quaternionf::Identity();
set_rotation_type(ViewerCore::ROTATION_TYPE_TWO_AXIS_VALUATOR_FIXED_UP);

// Defalut model viewing parameters
model_zoom = 1.0f;
model_translation << 0,0,0;

// Camera parameters
camera_zoom = 1.0f;
orthographic = false;
camera_view_angle = 45.0;
camera_dnear = 1.0;
camera_dfar = 100.0;
camera_eye << 0, 0, 5;
camera_center << 0, 0, 0;
camera_up << 0, 1, 0;

// Default visualization options
show_faces = true;
show_lines = true;
invert_normals = false;
show_overlay = true;
show_overlay_depth = true;
show_vertid = false;
show_faceid = false;
show_texture = false;
depth_test = true;

// Default point size / line width
point_size = 30;
line_width = 0.5f;
is_animating = false;
animation_max_fps = 30.;
*/









