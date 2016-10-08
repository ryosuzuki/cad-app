

#include "viewer.h"

Viewer::Viewer() : Screen(Eigen::Vector2i(1024, 768), "CAD App") {
  Window *window = new Window(this, "Viewer");
  window->setPosition(Eigen::Vector2i(15, 15));
  window->setLayout(new GroupLayout());

  Eigen::MatrixXf indices(3, 2);
  indices.col(0) << 0, 1, 2;
  indices.col(1) << 2, 3, 0;

  Eigen::MatrixXf positions(3, 4);
  positions.col(0) << -1, -1, 0;
  positions.col(1) <<  1, -1, 0;
  positions.col(2) <<  1,  1, 0;
  positions.col(3) << -1,  1, 0;

  shader_.init(
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
  shader_.bind();
  shader_.uploadIndices(indices);
  shader_.uploadAttrib("position", positions);
  shader_.setUniform("intensity", 0.5f);
}

Viewer::~Viewer() {
  shader_.free();
}

void Viewer::draw(NVGcontext *ctx) {
  Screen::draw(ctx);
}

void Viewer::drawContents() {
  shader_.bind();

  Eigen::Matrix4f mvp;
  mvp.setIdentity();
  mvp.topLeftCorner<3,3>() = Eigen::Matrix3f(Eigen::AngleAxisf((float) glfwGetTime(), Eigen::Vector3f::UnitZ())) * 0.25f;
  mvp.row(0) *= (float) mSize.y() / (float) mSize.x();

  shader_.setUniform("modelViewProj", mvp);

  /* Draw 2 triangles starting at index 0 */
  shader_.drawIndexed(GL_TRIANGLES, 0, 2);


}

