

#include "opengl.h"


void OpenGL::initBuffers() {
  glGenVertexArrays(1, &vaoMesh);
  glBindVertexArray(vaoMesh);
  glGenBuffers(1, &vboVertices);
  glGenBuffers(1, &vboVertexNormals);
  glGenBuffers(1, &vboVertexUvs);
  glGenBuffers(1, &vboFaces);
  glGenBuffers(1, &vboFaceAmbient);
  glGenBuffers(1, &vboFaceDiffuse);
  glGenBuffers(1, &vboFaceSpecular);
  glGenBuffers(1, &vboVertexAmbient);
  glGenBuffers(1, &vboVertexDiffuse);
  glGenBuffers(1, &vboVertexSpecular);
  glGenTextures(1, &vboTextures);
}

void OpenGL::freeBuffers() {
  glDeleteVertexArrays(1, &vaoMesh);
  glDeleteBuffers(1, &vboVertices);
  glDeleteBuffers(1, &vboVertexNormals);
  glDeleteBuffers(1, &vboVertexUvs);
  glDeleteBuffers(1, &vboFaces);
  glDeleteBuffers(1, &vboFaceAmbient);
  glDeleteBuffers(1, &vboFaceDiffuse);
  glDeleteBuffers(1, &vboFaceSpecular);
  glDeleteBuffers(1, &vboVertexAmbient);
  glDeleteBuffers(1, &vboVertexDiffuse);
  glDeleteBuffers(1, &vboVertexSpecular);
  glDeleteTextures(1, &vboTextures);
}


void OpenGL::setMesh(const Mesh &mesh) {
  vertices = (mesh.vertices.transpose()).cast<float>();
  vertexNormals = (mesh.vertexNormals.transpose()).cast<float>();
  faces = (mesh.faces.transpose()).cast<unsigned>();
  vertexUvs = (mesh.vertexUvs.transpose()).cast<float>();
  vertexAmbient = (mesh.vertexAmbient.transpose()).cast<float>();
  vertexDiffuse = (mesh.vertexDiffuse.transpose()).cast<float>();
  vertexSpecular = (mesh.vertexSpecular.transpose()).cast<float>();
}

void OpenGL::bindMesh() {
  glBindVertexArray(vaoMesh);
  shaderMesh.bind();
  shaderMesh.bindVertexAttribArray("position", vboVertices, vertices, true);
  shaderMesh.bindVertexAttribArray("normal", vboVertexNormals, vertexNormals, true);
  shaderMesh.bindVertexAttribArray("Ka", vboVertexAmbient, vertexAmbient, true);
  shaderMesh.bindVertexAttribArray("Kd", vboVertexDiffuse, vertexDiffuse, true);
  shaderMesh.bindVertexAttribArray("Ks", vboVertexSpecular, vertexSpecular, true);
  shaderMesh.bindVertexAttribArray("texcoord", vboVertexUvs, vertexUvs, true);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboFaces);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned)*faces.size(), faces.data(), GL_DYNAMIC_DRAW);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, vboTextures);
  // glUniform1i(shaderMesh.uniform("tex"), 0);
}

void OpenGL::drawMesh(bool solid) {
  glPolygonMode(GL_FRONT_AND_BACK, solid ? GL_FILL : GL_LINE);

  if (solid) {
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);
  }

  glDrawElements(GL_TRIANGLES, 3*faces.cols(), GL_UNSIGNED_INT, 0);

  glDisable(GL_POLYGON_OFFSET_FILL);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}


void OpenGL::init() {
  std::string meshVertexShaderString =
  "#version 150\n"
  "uniform mat4 model;"
  "uniform mat4 view;"
  "uniform mat4 proj;"
  "in vec3 position;"
  "in vec3 normal;"
  "out vec3 position_eye;"
  "out vec3 normal_eye;"
  "in vec3 Ka;"
  "in vec3 Kd;"
  "in vec3 Ks;"
  "in vec2 texcoord;"
  "out vec2 texcoordi;"
  "out vec3 Kai;"
  "out vec3 Kdi;"
  "out vec3 Ksi;"

  "void main()"
  "{"
  "  position_eye = vec3 (view * model * vec4 (position, 1.0));"
  "  normal_eye = vec3 (view * model * vec4 (normal, 0.0));"
  "  normal_eye = normalize(normal_eye);"
  "  gl_Position = proj * vec4 (position_eye, 1.0);" //proj * view * model * vec4(position, 1.0);"
  "  Kai = Ka;"
  "  Kdi = Kd;"
  "  Ksi = Ks;"
  "  texcoordi = texcoord;"
  "}";

  std::string meshFragmentShaderString =
  "#version 150\n"
  "uniform mat4 model;"
  "uniform mat4 view;"
  "uniform mat4 proj;"
  "uniform vec4 fixed_color;"
  "in vec3 position_eye;"
  "in vec3 normal_eye;"
  "uniform vec3 light_position_world;"
  "vec3 Ls = vec3 (1, 1, 1);"
  "vec3 Ld = vec3 (1, 1, 1);"
  "vec3 La = vec3 (1, 1, 1);"
  "in vec3 Ksi;"
  "in vec3 Kdi;"
  "in vec3 Kai;"
  "in vec2 texcoordi;"
  "uniform sampler2D tex;"
  "uniform float specular_exponent;"
  "uniform float lighting_factor;"
  "uniform float texture_factor;"
  "out vec4 outColor;"
  "void main()"
  "{"
  "vec3 Ia = La * Kai;"    // ambient intensity

  "vec3 light_position_eye = vec3 (view * vec4 (light_position_world, 1.0));"
  "vec3 vector_to_light_eye = light_position_eye - position_eye;"
  "vec3 direction_to_light_eye = normalize (vector_to_light_eye);"
  "float dot_prod = dot (direction_to_light_eye, normal_eye);"
  "float clamped_dot_prod = max (dot_prod, 0.0);"
  "vec3 Id = Ld * Kdi * clamped_dot_prod;"    // Diffuse intensity

  "vec3 reflection_eye = reflect (-direction_to_light_eye, normal_eye);"
  "vec3 surface_to_viewer_eye = normalize (-position_eye);"
  "float dot_prod_specular = dot (reflection_eye, surface_to_viewer_eye);"
  "dot_prod_specular = float(abs(dot_prod)==dot_prod) * max (dot_prod_specular, 0.0);"
  "float specular_factor = pow (dot_prod_specular, specular_exponent);"
  "vec3 Is = Ls * Ksi * specular_factor;"    // specular intensity
  "vec4 color = vec4(lighting_factor * (Is + Id) + Ia, 1.0) + vec4((1.0-lighting_factor) * Kdi,1.0);"
  "outColor = mix(vec4(1,1,1,1), texture(tex, texcoordi), texture_factor) * color;"
  "if (fixed_color != vec4(0.0)) outColor = fixed_color;"
  "}";

  initBuffers();
  shaderMesh.init(meshVertexShaderString, meshFragmentShaderString, "outColor");
}

void OpenGL::free() {
  shaderMesh.free();
  freeBuffers();
}

