#ifndef APPS_ALTERNATE_EMPTY_NESTED_MENU_CONTROLLER_H
#define APPS_ALTERNATE_EMPTY_NESTED_MENU_CONTROLLER_H

#include <escher/nested_menu_controller.h>

class AlternateEmptyNestedMenuController : public Escher::NestedMenuController {
public:
  AlternateEmptyNestedMenuController(I18n::Message title) :
    Escher::NestedMenuController(nullptr, title),
    m_isEmpty(false)
  {}
  // View Controller
  void viewDidDisappear() override;
protected:
  virtual Escher::ViewController * emptyViewController() = 0;
  bool isDisplayingEmptyController() { return m_isEmpty; }
  bool displayEmptyControllerIfNeeded();
  bool returnToPreviousMenu() override;
  bool m_isEmpty;
};

#endif
