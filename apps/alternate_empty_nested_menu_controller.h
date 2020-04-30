#ifndef APPS_ALTERNATE_EMPTY_NESTED_MENU_CONTROLLER_H
#define APPS_ALTERNATE_EMPTY_NESTED_MENU_CONTROLLER_H

#include <escher/nested_menu_controller.h>

class AlternateEmptyNestedMenuController : public NestedMenuController {
public:
  AlternateEmptyNestedMenuController(I18n::Message title) :
    NestedMenuController(nullptr, title)
  {}
  // View Controller
  void viewDidDisappear() override;
protected:
  virtual ViewController * emptyViewController() = 0;
  bool isDisplayingEmptyController() { return StackViewController::depth() == 2; }
  bool displayEmptyControllerIfNeeded();
};

#endif
