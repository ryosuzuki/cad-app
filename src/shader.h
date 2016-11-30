
#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <nanogui/opengl.h>

class GLShader {

public:
  /// Create an unitialized OpenGL shader
  GLShader()
    : mVertexShader(0), mFragmentShader(0), mGeometryShader(0),
      mProgramShader(0), mVertexArrayObject(0) { }

  bool init(const std::string &name, const std::string &vertex_str,
            const std::string &fragment_str,
            const std::string &geometry_str = "");

  bool initFromFiles(const std::string &name,
                     const std::string &vertex_fname,
                     const std::string &fragment_fname,
                     const std::string &geometry_fname = "");

  const std::string &name() const { return mName; }

  void define(const std::string &key, const std::string &value) { mDefinitions[key] = value; }
  void bind();
  void free();
  GLint attrib(const std::string &name, bool warn = true) const;
  GLint uniform(const std::string &name, bool warn = true) const;
  template <typename Matrix> void uploadAttrib(const std::string &name, const Matrix &M, int version = -1) {
    uint32_t compSize = sizeof(typename Matrix::Scalar);
    GLuint glType = (GLuint) detail::type_traits<typename Matrix::Scalar>::type;
    bool integral = (bool) detail::type_traits<typename Matrix::Scalar>::integral;

    uploadAttrib(name, (uint32_t) M.size(), (int) M.rows(), compSize,
                 glType, integral, M.data(), version);
  }
  template <typename Matrix> void downloadAttrib(const std::string &name, Matrix &M) {
    uint32_t compSize = sizeof(typename Matrix::Scalar);
    GLuint glType = (GLuint) detail::type_traits<typename Matrix::Scalar>::type;

    auto it = mBufferObjects.find(name);
    if (it == mBufferObjects.end())
      throw std::runtime_error("downloadAttrib(" + mName + ", " + name + ") : buffer not found!");

    const Buffer &buf = it->second;
    M.resize(buf.dim, buf.size / buf.dim);

    downloadAttrib(name, M.size(), M.rows(), compSize, glType, M.data());
  }

  template <typename Matrix> void uploadIndices(const Matrix &M, int version = -1) {
    uploadAttrib("indices", M, version);
  }

  void invalidateAttribs();

  void freeAttrib(const std::string &name);

  bool hasAttrib(const std::string &name) const {
    auto it = mBufferObjects.find(name);
    if (it == mBufferObjects.end())
      return false;
    return true;
  }

  void shareAttrib(const GLShader &otherShader, const std::string &name, const std::string &as = "");

  int attribVersion(const std::string &name) const {
    auto it = mBufferObjects.find(name);
    if (it == mBufferObjects.end())
      return -1;
    return it->second.version;
  }

  void resetAttribVersion(const std::string &name) {
    auto it = mBufferObjects.find(name);
    if (it != mBufferObjects.end())
      it->second.version = -1;
  }

  void drawArray(int type, uint32_t offset, uint32_t count);

  void drawIndexed(int type, uint32_t offset, uint32_t count);

  template <typename T>
  void setUniform(const std::string &name, const Eigen::Matrix<T, 4, 4> &mat, bool warn = true) {
    glUniformMatrix4fv(uniform(name, warn), 1, GL_FALSE, mat.template cast<float>().data());
  }

  template <typename T, typename std::enable_if<detail::type_traits<T>::integral == 1, int>::type = 0>
  void setUniform(const std::string &name, T value, bool warn = true) {
    glUniform1i(uniform(name, warn), (int) value);
  }

  /// Initialize a uniform parameter with a floating point value
  template <typename T, typename std::enable_if<detail::type_traits<T>::integral == 0, int>::type = 0>
  void setUniform(const std::string &name, T value, bool warn = true) {
    glUniform1f(uniform(name, warn), (float) value);
  }

  /// Initialize a uniform parameter with a 2D vector (int)
  template <typename T, typename std::enable_if<detail::type_traits<T>::integral == 1, int>::type = 0>
  void setUniform(const std::string &name, const Eigen::Matrix<T, 2, 1>  &v, bool warn = true) {
    glUniform2i(uniform(name, warn), (int) v.x(), (int) v.y());
  }

  /// Initialize a uniform parameter with a 2D vector (float)
  template <typename T, typename std::enable_if<detail::type_traits<T>::integral == 0, int>::type = 0>
  void setUniform(const std::string &name, const Eigen::Matrix<T, 2, 1>  &v, bool warn = true) {
    glUniform2f(uniform(name, warn), (float) v.x(), (float) v.y());
  }

  /// Initialize a uniform parameter with a 3D vector (int)
  template <typename T, typename std::enable_if<detail::type_traits<T>::integral == 1, int>::type = 0>
  void setUniform(const std::string &name, const Eigen::Matrix<T, 3, 1>  &v, bool warn = true) {
    glUniform3i(uniform(name, warn), (int) v.x(), (int) v.y(), (int) v.z());
  }

  /// Initialize a uniform parameter with a 3D vector (float)
  template <typename T, typename std::enable_if<detail::type_traits<T>::integral == 0, int>::type = 0>
  void setUniform(const std::string &name, const Eigen::Matrix<T, 3, 1>  &v, bool warn = true) {
    glUniform3f(uniform(name, warn), (float) v.x(), (float) v.y(), (float) v.z());
  }

  /// Initialize a uniform parameter with a 4D vector (int)
  template <typename T, typename std::enable_if<detail::type_traits<T>::integral == 1, int>::type = 0>
  void setUniform(const std::string &name, const Eigen::Matrix<T, 4, 1>  &v, bool warn = true) {
    glUniform4i(uniform(name, warn), (int) v.x(), (int) v.y(), (int) v.z(), (int) v.w());
  }

  /// Initialize a uniform parameter with a 4D vector (float)
  template <typename T, typename std::enable_if<detail::type_traits<T>::integral == 0, int>::type = 0>
  void setUniform(const std::string &name, const Eigen::Matrix<T, 4, 1>  &v, bool warn = true) {
    glUniform4f(uniform(name, warn), (float) v.x(), (float) v.y(), (float) v.z(), (float) v.w());
  }

  /// Initialize a uniform buffer with a uniform buffer object
  void setUniform(const std::string &name, const GLUniformBuffer &buf, bool warn = true);

  /// Return the size of all registered buffers in bytes
  size_t bufferSize() const {
    size_t size = 0;
    for (auto const &buf : mBufferObjects)
      size += buf.second.size;
    return size;
  }

public:
  /* Low-level API */
  void uploadAttrib(const std::string &name, size_t size, int dim,
                    uint32_t compSize, GLuint glType, bool integral,
                    const void *data, int version = -1);
  void downloadAttrib(const std::string &name, size_t size, int dim,
                      uint32_t compSize, GLuint glType, void *data);

protected:
  /**
   * \struct Buffer glutil.h nanogui/glutil.h
   *
   * A wrapper struct for maintaining various aspects of items being managed
   * by OpenGL.
   */
  struct Buffer {
    GLuint id;
    GLuint glType;
    GLuint dim;
    GLuint compSize;
    GLuint size;
    int version;
  };
  std::string mName;
  GLuint mVertexShader;
  GLuint mFragmentShader;
  GLuint mGeometryShader;
  GLuint mProgramShader;
  GLuint mVertexArrayObject;
  std::map<std::string, Buffer> mBufferObjects;
  std::map<std::string, std::string> mDefinitions;
};

//  ----------------------------------------------------

/**
 * \class GLUniformBuffer glutil.h nanogui/glutil.h
 *
 * \brief Helper class for creating OpenGL Uniform Buffer objects.
 */
class GLUniformBuffer {
public:
  /// Default constructor: unusable until you call the ``init()`` method
  GLUniformBuffer() : mID(0), mBindingPoint(0) { }

  /// Create a new uniform buffer
  void init();

  /// Release underlying OpenGL object
  void free();

  /// Bind the uniform buffer to a specific binding point
  void bind(int index);

  /// Release/unbind the uniform buffer
  void release();

  /// Update content on the GPU using data
  void update(const std::vector<uint8_t> &data);

  /// Return the binding point of this uniform buffer
  int getBindingPoint() const { return mBindingPoint; }
private:
  GLuint mID;
  int mBindingPoint;
};
