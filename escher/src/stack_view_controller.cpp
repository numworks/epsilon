extern "C" {
#include <assert.h>
}
#include <escher/stack_view_controller.h>
#include <escher/app.h>

StackViewController::ControllerView::ControllerView(bool displayFirstStackHeader) :
  View(),
  m_contentView(nullptr),
  m_numberOfStacks(0),
  m_displayFirstStackHeader(displayFirstStackHeader)
{
}

void StackViewController::ControllerView::setContentView(View * view) {
  m_contentView = view;
  layoutSubviews();
  markRectAsDirty(bounds());
}

void StackViewController::ControllerView::pushStack(ViewController * controller, KDColor textColor, KDColor backgroundColor, KDColor separatorColor) {
  m_stackViews[m_numberOfStacks].setNamedController(controller);
  m_stackViews[m_numberOfStacks].setTextColor(textColor);
  m_stackViews[m_numberOfStacks].setBackgroundColor(backgroundColor);
  m_stackViews[m_numberOfStacks].setSeparatorColor(separatorColor);
  m_numberOfStacks++;
}

void StackViewController::ControllerView::popStack() {
  assert(m_numberOfStacks > 0);
  m_numberOfStacks--;
}

void StackViewController::ControllerView::layoutSubviews() {
  KDCoordinate width = m_frame.width();
  int indexFirstHeader = m_displayFirstStackHeader ? 0 : 1;
  for (int i=indexFirstHeader; i<m_numberOfStacks; i++) {
    m_stackViews[i].setFrame(KDRect(0, k_stackHeight*(i-indexFirstHeader), width, k_stackHeight + 1));
  }
  if (m_contentView) {
    bool separatorHeight = m_displayFirstStackHeader + (m_numberOfStacks > 1);
    KDRect contentViewFrame = KDRect( 0, (m_numberOfStacks-indexFirstHeader)*k_stackHeight + separatorHeight,
        width, m_frame.height() - (m_numberOfStacks-indexFirstHeader)*k_stackHeight);
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

StackViewController::StackViewController(Responder * parentResponder, ViewController * rootViewController,
    bool displayFirstStackHeader, KDColor textColor, KDColor backgroundColor, KDColor separatorColor) :
  ViewController(parentResponder),
  m_view(ControllerView(displayFirstStackHeader)),
  m_numberOfChildren(0),
  m_rootViewController(rootViewController),
  m_textColor(textColor),
  m_backgroundColor(backgroundColor),
  m_separatorColor(separatorColor)
{
  // push(rootViewController);
}

const char * StackViewController::title() {
  if (m_rootViewController) {
    return m_rootViewController->title();
  } else {
    ViewController * vc = m_children[0];
    return vc->title();
  }
}

void StackViewController::push(ViewController * vc, KDColor textColor, KDColor backgroundColor, KDColor separatorColor) {
  m_view.pushStack(vc, textColor, backgroundColor, separatorColor);
  m_children[m_numberOfChildren++] = vc;
  if (m_numberOfChildren > 1) {
    m_children[m_numberOfChildren-2]->viewDidDisappear();
  }
  setupActiveViewController();
}

void StackViewController::pop() {
  m_view.popStack();
  assert(m_numberOfChildren > 0);
  ViewController * vc = m_children[m_numberOfChildren-1];
  vc->setParentResponder(nullptr);
  m_numberOfChildren--;
  vc->viewDidDisappear();
  setupActiveViewController();
}

void StackViewController::setupActiveViewController() {
  ViewController * vc = m_children[m_numberOfChildren-1];
  vc->setParentResponder(this);
  m_view.setContentView(vc->view());
  vc->viewWillAppear();
  app()->setFirstResponder(vc);
}

void StackViewController::didBecomeFirstResponder() {
  ViewController * vc = m_children[m_numberOfChildren-1];
  app()->setFirstResponder(vc);
}

bool StackViewController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back && m_numberOfChildren > 1) {
    pop();
    return true;
  }
  return false;
}

View * StackViewController::view() {
  return &m_view;
}

void StackViewController::viewWillAppear() {
  if (m_rootViewController != nullptr) {
    push(m_rootViewController, m_textColor, m_backgroundColor, m_separatorColor);
    m_rootViewController = nullptr;
  }
  ViewController * vc = m_children[m_numberOfChildren-1];
  if (m_numberOfChildren > 0 && vc) {
    vc->viewWillAppear();
  }
}

void StackViewController::viewDidDisappear() {
  ViewController * vc = m_children[m_numberOfChildren-1];
  if (m_numberOfChildren > 0 && vc) {
    vc->viewDidDisappear();
  }
}
