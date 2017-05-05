#ifndef ESCHER_DYNAMIC_VIEW_CONTROLLER_H
#define ESCHER_DYNAMIC_VIEW_CONTROLLER_H

#include <escher/view_controller.h>

/* Dynamic view controllers dynamically construct their views when appearring
 * and delete them when disappearing.
 * The load method is called before any methods viewWillAppear,
 * didEnterResponderChain and didBecomeFirstResponder. The unload method is
 * called after viewWillDisappear, willExitResponderChain or
 * willResignFirstResponder. */

class DynamicViewController : public ViewController {
public:
  DynamicViewController(Responder * parentResponder);
  ~DynamicViewController();
  DynamicViewController(const DynamicViewController& other) = delete;
  DynamicViewController(DynamicViewController&& other) = delete;
  DynamicViewController& operator=(const DynamicViewController& other) = delete;
  DynamicViewController& operator=(DynamicViewController&& other) = delete;
  View * view() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;
private:
  void loadViewIfNeeded();
  void unloadViewIfNeeded();
  virtual View * loadView() = 0;
  virtual void unloadView(View * view) = 0;
  View * m_view;
};

#endif
