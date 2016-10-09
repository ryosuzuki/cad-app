
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
  GLuint vboVertices; // Vertices of the current mesh (#V x 3)
  GLuint vboVertexUvs; // UV coordinates for the current mesh (#V x 2)
  GLuint vboVertexNormals; // Vertices of the current mesh (#V x 3)
  GLuint vboVertexAmbientMaterials; // Ambient material  (#V x 3)
  GLuint vboVertexDiffuseMaterials; // Diffuse material  (#V x 3)
  GLuint vboVertexSpecularMaterials; // Specular material  (#V x 3)

  GLuint vboFaces; // Faces of the mesh (#F x 3)
  GLuint vboTextures; // Texture

  // Temporary copy of the content of each VBO
  Eigen::MatrixXf vertices;
  Eigen::MatrixXf vertexNormals;
  Eigen::MatrixXf vertexAmbientMaterials;
  Eigen::MatrixXf vertexDiffuseMaterials;
  Eigen::MatrixXf vertexSpecularMaterials;
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

