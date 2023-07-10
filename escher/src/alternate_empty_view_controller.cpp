#include <assert.h>
#include <escher/alternate_empty_view_controller.h>
#include <escher/container.h>

namespace Escher {

/* ContentView */

AlternateEmptyViewController::ContentView::ContentView(
    ViewController* mainViewController, AlternateEmptyViewDelegate* delegate)
    : m_mainViewController(mainViewController), m_delegate(delegate) {}

View* AlternateEmptyViewController::ContentView::subviewAtIndex(int index) {
  assert(index == 0);
  if (isEmpty()) {
    return m_delegate->emptyView();
  }
  return m_mainViewController->view();
}

void AlternateEmptyViewController::ContentView::layoutSubviews(bool force) {
  if (isEmpty()) {
    setChildFrame(m_delegate->emptyView(), bounds(), force);
  } else {
    setChildFrame(m_mainViewController->view(), bounds(), force);
  }
}

/* AlternateEmptyViewController */

AlternateEmptyViewController::AlternateEmptyViewController(
    Responder* parentResponder, ViewController* mainViewController,
    AlternateEmptyViewDelegate* delegate)
    : ViewController(parentResponder),
      m_contentView(mainViewController, delegate) {}

const char* AlternateEmptyViewController::title() {
  return m_contentView.mainViewController()->title();
}

ViewController::TitlesDisplay AlternateEmptyViewController::titlesDisplay() {
  return m_contentView.mainViewController()->titlesDisplay();
}

void AlternateEmptyViewController::didBecomeFirstResponder() {
  if (!m_contentView.isEmpty()) {
    App::app()->setFirstResponder(m_contentView.mainViewController());
  } else {
    App::app()->setFirstResponder(
        m_contentView.alternateEmptyViewDelegate()->responderWhenEmpty());
  }
}

void AlternateEmptyViewController::initView() {
  m_contentView.mainViewController()->initView();
}

void AlternateEmptyViewController::viewWillAppear() {
  m_contentView.layoutSubviews();
  if (!m_contentView.isEmpty()) {
    m_contentView.mainViewController()->viewWillAppear();
  }
}

void AlternateEmptyViewController::viewDidDisappear() {
  if (!m_contentView.isEmpty()) {
    m_contentView.mainViewController()->viewDidDisappear();
  }
}

}  // namespace Escher
