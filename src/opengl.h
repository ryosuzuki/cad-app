
#pragma once

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>

#include "mesh.h"
#include "shader.h"

class OpenGL {
public:

  Shader shaderMesh;

  GLuint vaoMesh;
  GLuint vboVertices;
  GLuint vboVertexUvs;
  GLuint vboVertexNormals;
  GLuint vboVertexAmbient;
  GLuint vboVertexDiffuse;
  GLuint vboVertexSpecular;

  GLuint vboFaces;
  GLuint vboFaceAmbient;
  GLuint vboFaceDiffuse;
  GLuint vboFaceSpecular;
  GLuint vboTextures;


  Eigen::MatrixXf vertices;
  Eigen::MatrixXf vertexNormals;
  Eigen::MatrixXf vertexAmbient;
  Eigen::MatrixXf vertexDiffuse;
  Eigen::MatrixXf vertexSpecular;
  Eigen::MatrixXf vertexUvs;

  int u;
  int v;
  Eigen::Matrix<char,Eigen::Dynamic,1> textures;
  Eigen::Matrix<unsigned, Eigen::Dynamic, Eigen::Dynamic> faces;


  void init();
  void initBuffers();

  void setMesh(const Mesh &mesh);
  void bindMesh();
  void drawMesh(bool solid = true);

  void free();
  void freeBuffers();
};

