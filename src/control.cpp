
#include "control.h"

Eigen::Vector3f Control::project(const Eigen::Vector3f &obj, const Eigen::Matrix4f &model, const Eigen::Matrix4f &proj, const Eigen::Matrix4f &view, const Eigen::Vector4f &viewport) {

  Eigen::Vector4f tmp;
  tmp << obj, 1;
  tmp = view * model * tmp;
  tmp = proj * tmp;
  tmp = tmp.array() / tmp(3);
  tmp = tmp.array() * 0.5f + 0.5f;
  tmp(0) = tmp(0) * viewport(2) + viewport(0);
  tmp(1) = tmp(1) * viewport(3) + viewport(1);

  return tmp.head(3);
}

Eigen::Vector3f Control::unproject(const Eigen::Vector3f &win, const Eigen::Matrix4f &model, const Eigen::Matrix4f &proj, const Eigen::Matrix4f &view, const Eigen::Vector4f &viewport) {

  Eigen::Matrix4f Inverse = (proj * model).inverse();

  Eigen::Vector4f tmp;
  tmp << win, 1;
  tmp(0) = tmp(0) / viewport(2);
  tmp(1) = tmp(1) / viewport(3);
  tmp = tmp.array() * 2.0f - 1.0f;

  Eigen::Vector4f obj = Inverse * tmp;
  obj /= obj(3);

  return obj.head(3);
}

Eigen::Matrix4f Control::lookAt(const Eigen::Vector3f &origin, const Eigen::Vector3f &target, const Eigen::Vector3f &up) {

  Eigen::Vector3f f = (target - origin).normalized();
  Eigen::Vector3f s = f.cross(up).normalized();
  Eigen::Vector3f u = s.cross(f);

  Eigen::Matrix4f result = Eigen::Matrix4f::Identity();
  result(0, 0) = s(0);
  result(0, 1) = s(1);
  result(0, 2) = s(2);
  result(1, 0) = u(0);
  result(1, 1) = u(1);
  result(1, 2) = u(2);
  result(2, 0) = -f(0);
  result(2, 1) = -f(1);
  result(2, 2) = -f(2);
  result(0, 3) = -s.transpose() * origin;
  result(1, 3) = -u.transpose() * origin;
  result(2, 3) = f.transpose() * origin;
  return result;
}

Eigen::Matrix4f Control::ortho(float left, float right, float bottom, float top, float nearVal, float farVal) {

  Eigen::Matrix4f result = Eigen::Matrix4f::Identity();
  result(0, 0) = 2.0f / (right - left);
  result(1, 1) = 2.0f / (top - bottom);
  result(2, 2) = -2.0f / (farVal - nearVal);
  result(0, 3) = -(right + left) / (right - left);
  result(1, 3) = -(top + bottom) / (top - bottom);
  result(2, 3) = -(farVal + nearVal) / (farVal - nearVal);
  return result;
}

Eigen::Matrix4f Control::frustum(float left, float right, float bottom, float top, float nearVal, float farVal) {

  Eigen::Matrix4f result = Eigen::Matrix4f::Zero();
  result(0, 0) = (2.0f * nearVal) / (right - left);
  result(1, 1) = (2.0f * nearVal) / (top - bottom);
  result(0, 2) = (right + left) / (right - left);
  result(1, 2) = (top + bottom) / (top - bottom);
  result(2, 2) = -(farVal + nearVal) / (farVal - nearVal);
  result(3, 2) = -1.0f;
  result(2, 3) = -(2.0f * farVal * nearVal) / (farVal - nearVal);
  return result;
}

Eigen::Matrix4f Control::quatToMatrix(const Eigen::Quaternionf &quat) {
  Eigen::Matrix4f result = Eigen::Matrix4f::Identity();
  result.block<3,3>(0, 0) = quat.toRotationMatrix();
  return result;
}

Eigen::Matrix4f Control::scale(const Eigen::Vector3f &v) {
  return Eigen::Affine3f(Eigen::Scaling(v)).matrix();
}

Eigen::Matrix4f Control::translate(const Eigen::Vector3f &v) {
  return Eigen::Affine3f(Eigen::Translation<float, 3>(v)).matrix();
}

Eigen::Quaternionf Control::motion(float width, float height, float mouseX, float mouseY, float mouseDownX, float mouseDownY, float speed) {

  Eigen::Quaternionf result = Eigen::Quaternionf::Identity();

  float invMinDim = 1.0f / (width < height ? width : height);
  float ox = (speed * (2*mouseDownX - width) + width) - width - 1.0f;
  float tx = (speed * (2*mouseX - width) + width) - width - 1.0f;
  float oy = (speed * (height - 2*mouseDownY) + height) - height - 1.0f;
  float ty = (speed * (height - 2*mouseY) + height) - height - 1.0f;
  ox *= invMinDim;
  tx *= invMinDim;
  oy *= invMinDim;
  ty *= invMinDim;

  Eigen::Vector3f v0(ox, oy, 1.0f);
  Eigen::Vector3f v1(tx, ty, 1.0f);
  if (v0.squaredNorm() > 1e-4f && v1.squaredNorm() > 1e-4f) {
    v0.normalize();
    v1.normalize();
    Eigen::Vector3f axis = v0.cross(v1);
    float sa = std::sqrt(axis.dot(axis));
    float ca = v0.dot(v1);
    float angle = std::atan2(sa, ca);
    if (tx*tx + ty*ty > 1.0f) {
      angle *= 1.0f + 0.2f * (std::sqrt(tx*tx + ty*ty) - 1.0f);
    }
    result = Eigen::AngleAxisf(angle, axis.normalized());
    if (!std::isfinite(result.norm())) {
      result = Eigen::Quaternionf::Identity();
    }
  }
  return result;
}


