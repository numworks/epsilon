extern "C" {
#include <assert.h>
}
#include <escher/stack_view_controller.h>
#include <escher/app.h>

StackViewController::ControllerView::ControllerView() :
  View(),
  m_contentView(nullptr),
  m_numberOfStacks(0)
{
}

void StackViewController::ControllerView::setContentView(View * view) {
  m_contentView = view;
  layoutSubviews();
  markRectAsDirty(bounds());
}

void StackViewController::ControllerView::pushStack(const char * name) {
  m_stackViews[m_numberOfStacks].setName(name);
  m_numberOfStacks++;
}

void StackViewController::ControllerView::popStack() {
  assert(m_numberOfStacks > 0);
  m_numberOfStacks--;
}

void StackViewController::ControllerView::layoutSubviews() {
  KDCoordinate stackHeight = 20;
  KDCoordinate width = m_frame.width();
  for (int i=0; i<m_numberOfStacks; i++) {
    m_stackViews[i].setFrame(KDRect(0, stackHeight*i, width, stackHeight));
  }
  if (m_contentView) {
    KDRect contentViewFrame = KDRect( 0, m_numberOfStacks*stackHeight,
        width, m_frame.height() - m_numberOfStacks*stackHeight);
    m_contentView->setFrame(contentViewFrame);
  }
}

int StackViewController::ControllerView::numberOfSubviews() const {
  return m_numberOfStacks + (m_contentView == nullptr ? 0 : 1);
}

View * StackViewController::ControllerView::subviewAtIndex(int index) {
  if (index < m_numberOfStacks) {
    assert(index >= 0);
    return &m_stackViews[index];
  } else {
    assert(index == m_numberOfStacks);
    return m_contentView;
  }
}

#if ESCHER_VIEW_LOGGING
const char * StackViewController::ControllerView::className() const {
  return "StackViewController::ControllerView";
}
#endif

StackViewController::StackViewController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_numberOfChildren(0)
{
}

void StackViewController::push(ViewController * vc) {
  m_view.pushStack("name");
  m_children[m_numberOfChildren++] = vc;
  setupActiveViewController();
}

void StackViewController::pop() {
  m_view.popStack();
  assert(m_numberOfChildren > 0);
  m_numberOfChildren--;
  setupActiveViewController();
}

void StackViewController::setupActiveViewController() {
  ViewController * vc = m_children[m_numberOfChildren-1];
  vc->setParentResponder(this);
  m_view.setContentView(vc->view());
  app()->focus(vc);
}

void StackViewController::handleKeyEvent(int key) {
  // Switch tabs!
}


bool StackViewController::handleEvent(ion_event_t event) {
  return false;
}

View * StackViewController::view() {
  return &m_view;
}
