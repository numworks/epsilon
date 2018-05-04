#include <escher/bank_view_controller.h>
#include <escher/app.h>

BankViewController::BankViewController(Responder * parentViewController) :
  ViewController(parentViewController),
  m_activeIndex(0)
{
}

void BankViewController::setActiveIndex(int i) {
  assert(i >= 0 && i < numberOfChildren());
  if (i == m_activeIndex) {
    return;
  }
  ViewController * upcomingVC = childAtIndex(i);
  upcomingVC->viewWillAppear();
  app()->setFirstResponder(upcomingVC);
  childAtIndex(m_activeIndex)->viewDidDisappear();
  m_activeIndex = i;
  m_view.setSubview(upcomingVC->view());
}

void BankViewController::didEnterResponderChain(Responder * previousResponder) {
  app()->setFirstResponder(activeViewController());
}

void BankViewController::viewWillAppear() {
  m_view.setSubview(activeViewController()->view());
  activeViewController()->viewWillAppear();
}

void BankViewController::viewDidDisappear() {
  activeViewController()->viewDidDisappear();
}

void BankViewController::ContentView::setSubview(View * view) {
  m_subview = view;
  layoutSubviews();
  markRectAsDirty(bounds());
}
