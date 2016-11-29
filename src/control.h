
#pragma once

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>

extern Eigen::Vector3f project(const Eigen::Vector3f &obj,
                               const Eigen::Matrix4f &model,
                               const Eigen::Matrix4f &proj,
                               const Eigen::Vector2i &viewportSize);

extern Eigen::Vector3f unproject(const Eigen::Vector3f &win,
                                 const Eigen::Matrix4f &model,
                                 const Eigen::Matrix4f &proj,
                                 const Eigen::Vector2i &viewportSize);

extern Eigen::Matrix4f lookAt(const Eigen::Vector3f &origin,
                              const Eigen::Vector3f &target,
                              const Eigen::Vector3f &up);

extern Eigen::Matrix4f ortho(float left, float right,
                             float bottom, float top,
                             float nearVal, float farVal);

extern Eigen::Matrix4f frustum(float left, float right,
                               float bottom, float top,
                               float nearVal, float farVal);

extern Eigen::Matrix4f scale(const Eigen::Vector3f &v);

extern Eigen::Matrix4f translate(const Eigen::Vector3f &v);