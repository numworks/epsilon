#include <escher/bank_view_controller.h>
#include <escher/container.h>

namespace Escher {

BankViewController::BankViewController(Responder* parentViewController)
    : ViewController(parentViewController), m_activeIndex(0) {}

void BankViewController::setActiveIndex(int i) {
  assert(i >= 0 && i < numberOfChildren());
  if (i == m_activeIndex) {
    return;
  }
  ViewController* upcomingVC = childAtIndex(i);
  upcomingVC->viewWillAppear();
  Container::activeApp()->setFirstResponder(upcomingVC);
  childAtIndex(m_activeIndex)->viewDidDisappear();
  m_activeIndex = i;
  m_view.setSubview(upcomingVC->view());
}

void BankViewController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(activeViewController());
}

void BankViewController::initView() {
  int childrenNumber = numberOfChildren();
  for (int i = 0; i < childrenNumber; i++) {
    childAtIndex(i)->initView();
  }
  m_view.setSubview(activeViewController()->view());
}

void BankViewController::viewWillAppear() {
  activeViewController()->viewWillAppear();
}

void BankViewController::viewDidDisappear() {
  activeViewController()->viewDidDisappear();
}

void BankViewController::ContentView::setSubview(View* view) {
  m_subview = view;
  layoutSubviews();
  markRectAsDirty(bounds());
}

}  // namespace Escher
