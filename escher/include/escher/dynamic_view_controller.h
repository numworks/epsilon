#ifndef ESCHER_DYNAMIC_VIEW_CONTROLLER_H
#define ESCHER_DYNAMIC_VIEW_CONTROLLER_H

#include <escher/view_controller.h>

/* Dynamic view controllers dynamically construct their views when appearring and
 * delete them when disappearing. */

class DynamicViewController : public ViewController {
public:
  DynamicViewController(Responder * parentResponder);
  ~DynamicViewController();
  View * view() override;
  virtual void loadView() override;
  virtual void unloadView() override;
private:
  virtual View * createView() = 0;
  View * m_view;
};

#endif
