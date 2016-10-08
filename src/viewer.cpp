

#include "viewer.h"

Viewer::Viewer(bool fullscreen, bool deterministic)
    : Screen(Vector2i(1280, 960), "Instant Meshes", true, fullscreen),
      mOptimizer(mRes, true), mBVH(nullptr) {
    resizeEvent(mSize);
