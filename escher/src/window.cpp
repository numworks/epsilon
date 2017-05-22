#include <escher/window.h>
#include <ion.h>
extern "C" {
#include <assert.h>
}

Window::Window() :
  m_contentView(nullptr)
{
}

void Window::redraw(bool force) {
  if (force) {
    markRectAsDirty(bounds());
  }
  Ion::Display::waitForVBlank();
  View::redraw(bounds());
}

void Window::setContentView(View * contentView) {
  m_contentView = contentView;
  markRectAsDirty(bounds());
  layoutSubviews();
}

const Window * Window::window() const {
  return this;
}

int Window::numberOfSubviews() const {
  return (m_contentView == nullptr ? 0 : 1);
}

View * Window::subviewAtIndex(int index) {
  assert(m_contentView != nullptr && index == 0);
  return m_contentView;
}

void Window::layoutSubviews() {
  if (m_contentView != nullptr) {
    m_contentView->setFrame(bounds());
  }
}

#if ESCHER_VIEW_LOGGING
const char * Window::className() const {
  return "Window";
}
#endif
