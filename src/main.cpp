

#include "viewer.h"
#include <iostream>


int main() {
  std::cout << "Hello World";

  nanogui::init();
  {

    nanogui::ref<Viewer> viewer = new Viewer();
    viewer->drawAll();
    viewer->setVisible(true);

    nanogui::mainloop();
  }

  nanogui::shutdown();
  return 0;
}