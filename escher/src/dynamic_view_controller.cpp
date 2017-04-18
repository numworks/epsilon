#include <escher/dynamic_view_controller.h>
#include <escher/app.h>
#include <assert.h>

DynamicViewController::DynamicViewController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_view(nullptr)
{
}

DynamicViewController::~DynamicViewController() {
  if (m_view) {
    delete m_view;
    m_view = nullptr;
  }
}

View * DynamicViewController::view() {
  assert(m_view != nullptr);
  return m_view;
}

void DynamicViewController::loadView() {
  assert(m_view == nullptr);
  m_view = createView();
}

void DynamicViewController::unloadView() {
  assert(m_view != nullptr);
  delete m_view;
  m_view = nullptr;
}
