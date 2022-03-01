#ifndef ESCHER_ALTERNATE_VIEW_CONTROLLER_H
#define ESCHER_ALTERNATE_VIEW_CONTROLLER_H

#include <escher/i18n.h>
#include <escher/view_controller.h>
#include <escher/responder.h>

namespace Escher {

class AlternateViewDelegate {
public:
  virtual int activeViewControllerIndex() const = 0;
  virtual const char * alternateViewTitle() = 0;
  virtual ViewController::TitlesDisplay alternateViewTitlesDisplay() = 0;
};

class AlternateViewController : public ViewController {
public:
  AlternateViewController(Responder * parentResponder, AlternateViewDelegate * delegate, ViewController ** viewControllers);
  ViewController * activeViewController() { return m_viewControllers[m_delegate->activeViewControllerIndex()]; }
  View * view() override { return activeViewController()->view(); }
  const char * title() override { return m_delegate->alternateViewTitle(); }
  ViewController::TitlesDisplay titlesDisplay() override { return m_delegate->alternateViewTitlesDisplay(); }
  void didBecomeFirstResponder() override;
  void initView() override { activeViewController()->initView(); }
  void viewWillAppear() override;
  void viewDidDisappear() override { activeViewController()->viewDidDisappear(); }
private:
  AlternateViewDelegate * m_delegate;
  ViewController ** m_viewControllers;
};


}
#endif
