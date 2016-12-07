
#pragma once

#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>

class Control {
public:

  void init(const Eigen::Vector4f &viewport);
  void computeCameraMatries();
  void zoom(float delta);
  void updateRotation(float mouseX, float mouseY,
                      float mouseDownX, float mouseDownY,
                      const Eigen::Quaternionf &mouseDownRotation);
  Eigen::Vector3f project(const Eigen::Vector3f &obj);
  Eigen::Vector3f unproject(const Eigen::Vector3f &win);
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
  Eigen::Quaternionf getRotation();

  float width;
  float height;

  bool orthographic;
  float cameraViewAngle;
  float cameraNear;
  float cameraFar;
  float cameraZoom;
  float modelZoom;
  float speed;

  Eigen::Matrix4f model;
  Eigen::Matrix4f view;
  Eigen::Matrix4f proj;

  Eigen::Vector3f cameraEye;
  Eigen::Vector3f cameraCenter;
  Eigen::Vector3f cameraUp;
  Eigen::Vector3f modelTranslation;
  Eigen::Vector3f cameraLocal;
  Eigen::Quaternionf arcballQuat;

};