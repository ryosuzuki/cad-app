

#include "viewer.h"
#include <iostream>


int main() {
  std::cout << "Hello World" << std::endl;

  Viewer viewer;
  viewer.launch();

  nanogui::mainloop();
  nanogui::shutdown();
  return 0;
}