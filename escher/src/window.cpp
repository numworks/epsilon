#include <escher/window.h>
extern "C" {
#include <assert.h>
}

Window::Window() :
  m_contentView(nullptr)
{
}

const Window * Window::window() const {
  return this;
}

int Window::numberOfSubviews() const {
  return (m_contentView == nullptr ? 0 : 1);
}

const View * Window::subview(int index) const {
  assert(m_contentView != nullptr && index == 0);
  return m_contentView;
}

void Window::layoutSubviews() {
  if (m_contentView != nullptr) {
    m_contentView->setFrame(this->bounds());
  }
}

void Window::storeSubviewAtIndex(View * view, int index) {
  assert(index == 0);
  m_contentView = view;
}

#if ESCHER_VIEW_LOGGING
const char * Window::className() const {
  return "Window";
}
#endif
