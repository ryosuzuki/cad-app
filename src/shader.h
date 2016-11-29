
#pragma once

#include <Eigen/Core>
// #include <GL/glew.h>
#include <OpenGL/gl3.h>
#include <iostream>
#include <fstream>

class Shader {
public:

  GLuint vertexShader;
  GLuint fragmentShader;
  GLuint geometryShader;
  GLuint programShader;

  bool init(const std::string &vertexShaderString,
    const std::string &fragmentShaderString,
    const std::string &fragmentDataName,
    const std::string &geometryShaderString = "",
    int geometryShaderMaxVertices = 3);
  void bind();
  void free();

  GLint attrib(const std::string &name) const;
  GLint uniform(const std::string &name) const;
  GLint bindVertexAttribArray(const std::string &name, GLuint bufferID, const Eigen::MatrixXf &M, bool refresh) const;
  GLuint createShaderHelper(GLint type, const std::string &shaderString);

};

