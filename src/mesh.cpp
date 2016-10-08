

#include "mesh.h"

void Mesh::set(const Eigen::MatrixXd &vertices, const Eigen::MatrixXi &faces) {
  this->vertices = vertices;
  this->faces = faces;
}

// void Mesh::setUv(const Eigen::MatrixXd &vertexUvs, const Eigen::MatrixXi &faceUvs) {
//   this->vertexUvs = vertexUvs;
//   this->faceUvs = faceUvs;
// }

// void Mesh::computeNormals() {
//   igl::per_face_normals(vertices, faces, faceNormals);
//   igl::per_vertex_normals(vertices, faces, vertexNormals);
// }