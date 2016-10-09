
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

  // Initialize shaders and buffers
  void init();

  // Release all resources
  void free();

  // Create a new set of OpenGL buffer objects
  void initBuffers();

  // Update contents from a 'Data' instance
  void setMesh(const Mesh &mesh);

  // Bind the underlying OpenGL buffer objects for subsequent mesh draw calls
  void bindMesh();

  /// Draw the currently buffered mesh (either solid or wireframe)
  void drawMesh(bool solid);

  // Release the OpenGL buffer objects
  void freeBuffers();
};

