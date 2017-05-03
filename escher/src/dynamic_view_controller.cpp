#include <escher/dynamic_view_controller.h>
#include <escher/app.h>
#include <assert.h>

DynamicViewController::DynamicViewController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_view(nullptr)
{
}

DynamicViewController::~DynamicViewController() {
  assert(m_view == nullptr);
}

View * DynamicViewController::view() {
  loadViewIfNeeded();
  return m_view;
}

void DynamicViewController::viewWillAppear() {
  loadViewIfNeeded();
}

void DynamicViewController::viewDidDisappear() {
  unloadViewIfNeeded();
}

void DynamicViewController::loadViewIfNeeded() {
  if (m_view == nullptr) {
    m_view = loadView();
    assert(m_view != nullptr);
  }
}

void DynamicViewController::unloadViewIfNeeded() {
  if (m_view != nullptr) {
    unloadView(m_view);
    m_view = nullptr;
  }
}
