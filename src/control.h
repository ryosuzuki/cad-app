
#pragma once

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>

class Control {
public:
Eigen::Vector3f project(const Eigen::Vector3f &obj,
                        const Eigen::Matrix4f &model,
                        const Eigen::Matrix4f &proj,
                        const Eigen::Matrix4f &view,
                        const Eigen::Vector4f &viewport);

Eigen::Vector3f unproject(const Eigen::Vector3f &win,
                          const Eigen::Matrix4f &model,
                          const Eigen::Matrix4f &proj,
                          const Eigen::Matrix4f &view,
                          const Eigen::Vector4f &viewport);

Eigen::Matrix4f lookAt(const Eigen::Vector3f &origin,
                       const Eigen::Vector3f &target,
                       const Eigen::Vector3f &up);

Eigen::Matrix4f ortho(float left, float right,
                      float bottom, float top,
                      float nearVal, float farVal);

Eigen::Matrix4f frustum(float left, float right,
                        float bottom, float top,
                        float nearVal, float farVal);

Eigen::Matrix4f scale(const Eigen::Vector3f &v);

Eigen::Matrix4f translate(const Eigen::Vector3f &v);

Eigen::Matrix4f quatToMatrix(const Eigen::Quaternionf &quat);

Eigen::Quaternionf motion(float width, float height,
                          float mouseX, float mouseY,
                          float mouseDownX, float mouseDownY,
                          float speed);

};