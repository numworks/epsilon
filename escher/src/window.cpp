#include <escher/window.h>
extern "C" {
#include <assert.h>
}

Window::Window() :
  m_contentView(nullptr)
{
}

void Window::redraw() {
  View::redraw(bounds());
}

void Window::setContentView(View * contentView) {
  if (m_contentView != contentView) {
    m_contentView = contentView;
    markRectAsDirty(bounds());
  }
};

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
    m_contentView->setFrame(this->bounds());
  }
}

#if ESCHER_VIEW_LOGGING
const char * Window::className() const {
  return "Window";
}
#endif
