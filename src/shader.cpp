

#include "shader.h"

bool Shader::init(const std::string &vertexShaderString,
  const std::string &fragmentShaderString,
  const std::string &fragmentDataName,
  const std::string &geometryShaderString,
  int geometryShaderMaxVertices)
{
  using namespace std;
  vertexShader = createShaderHelper(GL_VERTEX_SHADER, vertexShaderString);
  geometryShader = createShaderHelper(GL_GEOMETRY_SHADER, geometryShaderString);
  fragmentShader = createShaderHelper(GL_FRAGMENT_SHADER, fragmentShaderString);

  if (!vertexShader || !fragmentShader) {
    return false;
  }

  programShader = glCreateProgram();

  glAttachShader(programShader, vertexShader);
  glAttachShader(programShader, fragmentShader);

  if (geometryShader) {
    glAttachShader(programShader, geometryShader);

    /* This covers only basic cases and may need to be modified */
    glProgramParameteri(programShader, GL_GEOMETRY_INPUT_TYPE, GL_TRIANGLES);
    glProgramParameteri(programShader, GL_GEOMETRY_OUTPUT_TYPE, GL_TRIANGLES);
    glProgramParameteri(programShader, GL_GEOMETRY_VERTICES_OUT, geometryShaderMaxVertices);
  }

  glBindFragDataLocation(programShader, 0, fragmentDataName.c_str());
  glLinkProgram(programShader);

  GLint status;
  glGetProgramiv(programShader, GL_LINK_STATUS, &status);

  if (status != GL_TRUE) {
    char buffer[512];
    glGetProgramInfoLog(programShader, 512, NULL, buffer);
    std::cerr << "Linker error: " << std::endl << buffer << std::endl;
    programShader = 0;
    return false;
  }

  return true;
}

void Shader::bind() {
  glUseProgram(programShader);
}

GLint Shader::attrib(const std::string &name) const {
  return glGetAttribLocation(programShader, name.c_str());
}

GLint Shader::uniform(const std::string &name) const {
  return glGetUniformLocation(programShader, name.c_str());
}

GLint Shader::bindVertexAttribArray(const std::string &name, GLuint bufferID, const Eigen::MatrixXf &M, bool refresh) const {
  GLint id = attrib(name);
  if (id < 0)
    return id;
  if (M.size() == 0) {
    glDisableVertexAttribArray(id);
    return id;
  }
  glBindBuffer(GL_ARRAY_BUFFER, bufferID);
  if (refresh)
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*M.size(), M.data(), GL_DYNAMIC_DRAW);
  glVertexAttribPointer(id, M.rows(), GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(id);
  return id;
}

void Shader::free() {
  if (programShader) {
    glDeleteProgram(programShader);
    programShader = 0;
  }
  if (vertexShader) {
    glDeleteShader(vertexShader);
    vertexShader = 0;
  }
  if (fragmentShader) {
    glDeleteShader(fragmentShader);
    fragmentShader = 0;
  }
  if (geometryShader) {
    glDeleteShader(geometryShader);
    geometryShader = 0;
  }
}

GLuint Shader::createShaderHelper(GLint type, const std::string &shaderString) {
  using namespace std;
  if (shaderString.empty()) {
    return (GLuint) 0;
  }

  GLuint id = glCreateShader(type);
  const char *shaderStringConst = shaderString.c_str();
  glShaderSource(id, 1, &shaderStringConst, NULL);
  glCompileShader(id);

  GLint status;
  glGetShaderiv(id, GL_COMPILE_STATUS, &status);

  if (status != GL_TRUE) {
    char buffer[512];
    if (type == GL_VERTEX_SHADER)
      std::cerr << "Vertex shader:" << std::endl;
    else if (type == GL_FRAGMENT_SHADER)
      std::cerr << "Fragment shader:" << std::endl;
    else if (type == GL_GEOMETRY_SHADER)
      std::cerr << "Geometry shader:" << std::endl;
    std::cerr << shaderString <<std::endl << std::endl;
    glGetShaderInfoLog(id, 512, NULL, buffer);
    std::cerr << "Error: " << std::endl << buffer << std::endl;
    return (GLuint) 0;
  }

  return id;
}