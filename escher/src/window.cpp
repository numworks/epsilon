#include <escher/window.h>
extern "C" {
#include <assert.h>
}

Window::Window() :
  m_contentView(nullptr)
{
}

bool Window::isOnScreen() {
  return true;
}

int Window::numberOfSubviews() {
  return (m_contentView == nullptr ? 0 : 1);
}

View * Window::subview(int index) {
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
