
#include "ray.h"

void Ray::init(const Mesh &mesh, const Eigen::Vector3f &origin_, const Eigen::Vector3f &direction_) {
  V = mesh.V;
  F = mesh.F;
  N = mesh.N;
  nodes = mesh.nodes;

  origin = origin_;
  direction = direction_;

  minTime = 0.0f;
  maxTime = std::numeric_limits<float>::infinity();
}

bool Ray::intersect(int &faceId, float &t, Eigen::Vector2f *uv) {
  if (nodes.empty()) {
    return false;
  }

  int nodeId = 0;
  int stackId = 0;
  int stack[64];
  bool hit = false;
  t = std::numeric_limits<float>::infinity();

  while (true) {
    const BVHNode &node = nodes[nodeId];

    if (!intersectAABB(node.aabb)) {
      if (stackId == 0) {
        break;
      }
      nodeId = stack[--stackId];
      continue;
    }

    if (!node.leaf.flag) {
      stack[stackId++] = node.inner.rightChild;
      nodeId++;
    } else {
      float _t;
      Eigen::Vector2f _uv;
      for (int fi = node.leaf.startId; fi < node.leaf.endId; ++fi) {
        if (intersectFace(fi, _t, _uv)) {
          faceId = fi;
          t = maxTime = _t;
          hit = true;
          if (uv) {
            *uv = _uv;
          }
        }
      }
      if (stackId == 0) {
        break;
      }
      nodeId = stack[--stackId];
      continue;
    }
  }
  return hit;
}

bool Ray::intersectAABB(const AABB &aabb) {
  float nearTime = -std::numeric_limits<float>::infinity();
  float farTime = std::numeric_limits<float>::infinity();

  for (int i = 0; i < 3; i++) {
    if (direction[i] == 0) {
      if (origin[i] < aabb.min[i] || origin[i] > aabb.max[i])
        return false;
    } else {
      float t1 = (aabb.min[i] - origin[i]) / direction[i];
      float t2 = (aabb.max[i] - origin[i]) / direction[i];

      if (t1 > t2) {
        std::swap(t1, t2);
      }

      nearTime = std::max(t1, nearTime);
      farTime = std::min(t2, farTime);

      if (!(nearTime <= farTime))
        return false;
    }
  }

  return minTime <= farTime && nearTime <= maxTime;
}

bool Ray::intersectFace(int faceId, float &t, Eigen::Vector2f &uv) {
  int a = F(0, faceId);
  int b = F(1, faceId);
  int c = F(2, faceId);
  Eigen::Vector3f va = V.col(a);
  Eigen::Vector3f vb = V.col(b);
  Eigen::Vector3f vc = V.col(c);

  Eigen::Vector3f edge1 = vb - va;
  Eigen::Vector3f edge2 = vc - va;
  Eigen::Vector3f pvec = direction.cross(edge2);

  float det = edge1.dot(pvec);
  if (det == 0.0f) {
    return false;
  }
  float inv_det = 1.0f / det;

  Eigen::Vector3f tvec = origin - va;
  float u = tvec.dot(pvec) * inv_det;
  if (u < 0.0f || u > 1.0f) {
    return false;
  }

  Eigen::Vector3f qvec = tvec.cross(edge1);
  float v = direction.dot(qvec) * inv_det;
  if (v < 0.0f || u + v > 1.0f) {
    return false;
  }

  float time = edge2.dot(qvec) * inv_det;
  if (time < minTime || time > maxTime) {
    return false;
  }

  t = time;
  uv << u, v;
  return true;
}
