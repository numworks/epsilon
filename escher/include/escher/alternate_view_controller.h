#ifndef ESCHER_ALTERNATE_VIEW_CONTROLLER_H
#define ESCHER_ALTERNATE_VIEW_CONTROLLER_H

#include <escher/i18n.h>
#include <escher/view_controller.h>
#include <escher/responder.h>

namespace Escher {

class AlternateViewDelegate {
public:
  virtual int activeViewControllerIndex() const = 0;
};

class AlternateViewController : public ViewController {
public:
  AlternateViewController(Responder * parentResponder, AlternateViewDelegate * delegate, ViewController ** viewControllers, const char * title);
  ViewController * activeViewController() { return m_viewControllers[m_delegate->activeViewControllerIndex()]; }
  View * view() override { return activeViewController()->view(); }
  const char * title() override { return m_title; }
  ViewController::TitlesDisplay titlesDisplay() override { return TitlesDisplay::DisplayNoTitle; }
  void didBecomeFirstResponder() override;
  void initView() override { activeViewController()->initView(); }
  void viewWillAppear() override;
  void viewDidDisappear() override { activeViewController()->viewDidDisappear(); }
private:
  AlternateViewDelegate * m_delegate;
  ViewController ** m_viewControllers;
  const char * m_title;
};


}
#endif
