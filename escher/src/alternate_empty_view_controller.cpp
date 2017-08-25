#include <escher/alternate_empty_view_controller.h>
#include <escher/app.h>
#include <escher/palette.h>
#include <assert.h>

/* ContentView */

AlternateEmptyViewController::ContentView::ContentView(ViewController * mainViewController, AlternateEmptyViewDelegate * delegate) :
  m_message(KDText::FontSize::Small, (I18n::Message)0, 0.5f, 0.5f, KDColorBlack, Palette::WallScreen),
  m_mainViewController(mainViewController),
  m_delegate(delegate)
{
}

int AlternateEmptyViewController::ContentView::numberOfSubviews() const {
  return 1;
}

View * AlternateEmptyViewController::ContentView::subviewAtIndex(int index) {
  assert(index == 0);
  if (m_delegate->isEmpty()) {
    m_message.setMessage(m_delegate->emptyMessage());
    return &m_message;
  }
  return m_mainViewController->view();
}

void AlternateEmptyViewController::ContentView::layoutSubviews() {
  if (alternateEmptyViewDelegate()->isEmpty()) {
    m_message.setFrame(bounds());
  } else {
    m_mainViewController->view()->setFrame(bounds());
  }
}

ViewController * AlternateEmptyViewController::ContentView::mainViewController() const {
  return m_mainViewController;
}

AlternateEmptyViewDelegate * AlternateEmptyViewController::ContentView::alternateEmptyViewDelegate() const {
  return m_delegate;
}

/* AlternateEmptyViewController */

AlternateEmptyViewController::AlternateEmptyViewController(Responder * parentResponder, ViewController * mainViewController, AlternateEmptyViewDelegate * delegate) :
  ViewController(parentResponder),
  m_contentView(mainViewController, delegate)
{
}

View * AlternateEmptyViewController::view() {
  return &m_contentView;
}

const char * AlternateEmptyViewController::title() {
  return m_contentView.mainViewController()->title();
}

bool AlternateEmptyViewController::handleEvent(Ion::Events::Event event) {
  if (m_contentView.alternateEmptyViewDelegate()->isEmpty()) {
    if (event != Ion::Events::Home && event != Ion::Events::OnOff) {
      app()->setFirstResponder(m_contentView.alternateEmptyViewDelegate()->defaultController());
      return true;
    }
    return false;
  }
  return false;
}

void AlternateEmptyViewController::didBecomeFirstResponder() {
  if (!m_contentView.alternateEmptyViewDelegate()->isEmpty()) {
    app()->setFirstResponder(m_contentView.mainViewController());
  }
}

void AlternateEmptyViewController::viewWillAppear() {
  m_contentView.layoutSubviews();
  if (!m_contentView.alternateEmptyViewDelegate()->isEmpty()) {
    m_contentView.mainViewController()->viewWillAppear();
  }
}

void AlternateEmptyViewController::viewDidDisappear() {
  if (!m_contentView.alternateEmptyViewDelegate()->isEmpty()) {
    m_contentView.mainViewController()->viewDidDisappear();
  }
}
