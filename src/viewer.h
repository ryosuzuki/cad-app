
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <nanogui/nanogui.h>

using nanogui::Alignment;
using nanogui::Arcball;
using nanogui::BoxLayout;
using nanogui::Button;
using nanogui::CheckBox;
using nanogui::Color;
using nanogui::ComboBox;
using nanogui::FormHelper;
using nanogui::GLFramebuffer;
using nanogui::GroupLayout;
using nanogui::ImagePanel;
using nanogui::Label;
using nanogui::MessageDialog;
using nanogui::Orientation;
using nanogui::Popup;
using nanogui::PopupButton;
using nanogui::ProgressBar;
using nanogui::Screen;
using nanogui::Slider;
using nanogui::TextBox;
using nanogui::ToolButton;
using nanogui::VScrollPanel;
using nanogui::Widget;
using nanogui::Window;
using nanogui::frustum;
using nanogui::lookAt;
using nanogui::project;
using nanogui::scale;
using nanogui::translate;
using nanogui::unproject;
using nanogui::utf8;

class Viewer : public Screen {
public:
  Viewer();
  virtual ~Viewer();

protected:
  nanogui::GLShader shader_;
};

