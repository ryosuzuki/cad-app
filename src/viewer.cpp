

#include "viewer.h"

void Viewer::init() {

  Screen *screen = new Screen(Eigen::Vector2i(1024, 768), "CAD app");
  screen->drawAll();
  screen->setVisible(true);

  Window *window = new Window(screen, "Menu");
  window->setPosition(Eigen::Vector2i(15, 15));
  window->setLayout(new GroupLayout());
  window->setId("viewer");

}

void Viewer::launch() {
  init();
}


/*
  Window *window = new Window(this, "Menu");
  window->setPosition(Eigen::Vector2i(15, 15));
  window->setLayout(new GroupLayout());
  window->setId("viewer");

  PopupButton *openButton = new PopupButton(window, "Open Mesh");
  openButton->setBackgroundColor(Color(0, 255, 0, 25));
  // openButton->setIcon(ENTYPO_ICON_FOLDER);
  Popup *popup = openButton->popup();
  VScrollPanel *vscroll = new VScrollPanel(popup);
  ImagePanel *panel = new ImagePanel(vscroll);
  // panel->setImages(mExampleImages);
  panel->setCallback([&, openButton](int i) {
    openButton->setPushed(false);
    // loadInput(mExampleImages[i].second);
  });

 */