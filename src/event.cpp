
#include "event.h"

void init() {
  glfwSetKeyCallback(window, [](GLFWwindow *, int key, int scancode, int action, int mods) {
    screen->keyCallbackEvent(key, scancode, action, mods);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
  });

  glfwSetCharCallback(window, [](GLFWwindow *, unsigned int codepoint) {
    screen->charCallbackEvent(codepoint);
  });

  glfwSetDropCallback(window, [](GLFWwindow *, int count, const char **filenames) {
    screen->dropCallbackEvent(count, filenames);
    // load(filenames[0]);
  });

  glfwSetMouseButtonCallback(window, [](GLFWwindow *, int button, int action, int modifiers) {
    screen->mouseButtonCallbackEvent(button, action, modifiers);

    if (action == GLFW_PRESS) {
      mouseDown = true;
      Eigen::Vector3f coord = control.project(center, modelMatrix, viewMatrix, projMatrix, viewport);

      mouseDownX = currentMouseX;
      mouseDownY = currentMouseY;
      mouseDownZ = coord[2];
      mouseDownRotation = arcballQuat.normalized();

      if (selectVertexId != -1) {
        float zval = control.project(mesh.weightedCenter, modelMatrix, viewMatrix, projMatrix, viewport).z();
        Eigen::Vector3f mouse = { currentMouseX, height - currentMouseY, zval };
        Eigen::Vector3f position = control.unproject(mouse, modelMatrix, viewMatrix, projMatrix, viewport);
        Eigen::Vector3f vertex = mesh.V.col(selectVertexId);

        float distance = (vertex - position).norm() / (mesh.boundingBox.max - mesh.boundingBox.min).norm();
        std::cout << distance << std::endl;
      }

    } else {
      mouseDown = false;
    }

    if (currentVertexId != -1) {
      selectVertexId = currentVertexId;
    }

  });

  glfwSetCursorPosCallback(window, [](GLFWwindow *, double x, double y) {
    screen->cursorPosCallbackEvent(x, y);

    float mouseX = x;
    float mouseY = y;

    currentMouseX = x;
    currentMouseY = y;

    double speed = 2.0;

    if (mouseDown) {
      if (deformation) {
        if (selectVertexId == -1) {
          return;
        }
        float zval = control.project(mesh.weightedCenter, modelMatrix, viewMatrix, projMatrix, viewport).z();
        Eigen::Vector3f mouse = { currentMouseX, height - currentMouseY, zval };
        Eigen::Vector3f position = control.unproject(mouse, modelMatrix, viewMatrix, projMatrix, viewport);

        deform.setConstraint(selectVertexId, position);
        deform.solve(mesh.V);
      } else {
        Eigen::Quaternionf diffRotation = control.motion(width, height, mouseX, mouseY, mouseDownX, mouseDownY, speed);
        arcballQuat = diffRotation * mouseDownRotation;
      }

    }

    Eigen::Vector3f p0 = { currentMouseX, height - currentMouseY, 0.0f };
    Eigen::Vector3f p1 = { currentMouseX, height - currentMouseY, 1.0f };
    Eigen::Vector3f pos0 = control.unproject(p0, modelMatrix, viewMatrix, projMatrix, viewport);
    Eigen::Vector3f pos1 = control.unproject(p1, modelMatrix, viewMatrix, projMatrix, viewport);
    ray.set(pos0, (pos1 - pos0).normalized());

    float minTime = std::numeric_limits<float>::infinity();
    Eigen::Vector2f uv;

    if (selectVertexId == -1) {
      for (int i = 0; i < mesh.F.cols(); ++i) {
        float time;
        bool hit = ray.intersectFace(i, time, uv);
        if (hit && time < minTime) {
          currentFaceId = i;
          minTime = time;
          // std::cout << uv << std::endl;
        }
        if (i == mesh.F.cols()) {
          currentFaceId = -1;
        }
        mesh.setColor(mesh.F(0, i), unselectColor);
        mesh.setColor(mesh.F(1, i), unselectColor);
        mesh.setColor(mesh.F(2, i), unselectColor);
      }
    }

    if (currentFaceId != -1 && selectVertexId == -1) {
      currentVertexId = mesh.F(0, currentFaceId);
      mesh.setColor(currentVertexId, selectColor);
      // mesh.setColor(mesh.F(1, currentFaceId), selectColor);
      // mesh.setColor(mesh.F(2, currentFaceId), selectColor);
    }


  });

  glfwSetScrollCallback(window, [](GLFWwindow *, double x, double y) {
    screen->scrollCallbackEvent(x, y);

    float deltaY = y;
    if (deltaY != 0) {
      float mult = (1.0 + ((deltaY > 0) ? 1.0 : -1.0) * 0.05);
      const float minZoom = 0.1f;
      cameraZoom = (cameraZoom * mult > minZoom ? cameraZoom * mult : minZoom);
    }

  });

  glfwSetFramebufferSizeCallback(window, [](GLFWwindow *, int width, int height) {
    screen->resizeCallbackEvent(width, height);
  });
}