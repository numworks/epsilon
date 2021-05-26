extern "C" {
#include <assert.h>
}
#include <escher/container.h>
#include <escher/metric.h>
#include <escher/stack_view_controller.h>

namespace Escher {

StackViewController::ControllerView::ControllerView()
    : View(),
      m_borderView(Palette::GrayBright),
      m_contentView(nullptr),
      m_numberOfStacks(0),
      m_displayMask(~0),
      m_headersOverlapHeaders(true),
      m_headersOverlapContent(false) {}

void StackViewController::ControllerView::setContentView(View * view) {
  m_contentView = view;
  layoutSubviews();
  markRectAsDirty(bounds());
}

void StackViewController::ControllerView::setupHeadersBorderOverlaping(
    bool headersOverlapHeaders, bool headersOverlapContent, KDColor headersContentBorderColor) {
  m_headersOverlapHeaders = headersOverlapHeaders;
  m_headersOverlapContent = headersOverlapContent;
  m_borderView.setColor(headersContentBorderColor);
}

void StackViewController::ControllerView::pushStack(Frame frame) {
  m_stackViews[m_numberOfStacks].setNamedController(frame.viewController());
  m_stackViews[m_numberOfStacks].setTextColor(frame.textColor());
  m_stackViews[m_numberOfStacks].setBackgroundColor(frame.backgroundColor());
  m_stackViews[m_numberOfStacks].setSeparatorColor(frame.separatorColor());
  m_numberOfStacks++;
}

void StackViewController::ControllerView::popStack() {
  assert(m_numberOfStacks > 0);
  m_numberOfStacks--;
}

bool StackViewController::ControllerView::isHeaderDisplayed(int i) const {
  return maskBit(m_numberOfStacks - 1 - i);
}

void StackViewController::ControllerView::layoutSubviews(bool force) {
  // Compute view frames
  KDCoordinate width = m_frame.width();
  int heightOffset = 0;
  int heightDiff = Metric::StackTitleHeight + (m_headersOverlapHeaders ? 0 : Metric::CellSeparatorThickness);
  for (int i = 0; i < m_numberOfStacks; i++) {
    if (isHeaderDisplayed(i)) {
      // Account for separator thickness in position only if there is no overlap
      m_stackViews[i].setFrame(KDRect(0,heightOffset, width, Metric::StackTitleHeight + Metric::CellSeparatorThickness),
                               force);
      heightOffset += heightDiff;
    }
  }
  if (m_contentView) {
    heightOffset += Metric::CellSeparatorThickness;
    if (borderShouldOverlapContent()) {
      if (!m_headersOverlapHeaders) {
        heightOffset -= Metric::CellSeparatorThickness;
      }
      // Layout the common border (which will override content)
      m_borderView.setFrame(KDRect(0, heightOffset, width, Metric::CellSeparatorThickness),
                            force);
    }
    // Layout content view
    KDRect contentViewFrame = KDRect(0, heightOffset, width, m_frame.height() - heightOffset);
    m_contentView->setFrame(contentViewFrame, force);
  }
}

bool StackViewController::ControllerView::borderShouldOverlapContent() const {
  /* When content is bordered, an additional border may be drawn between content
   * and headers, on top of everything, to preserve a clean separation when the
   * content scrolls.
   * This border should only be added if all these conditions are satisfied :
   *  - Headers should overlap content :        m_headersOverlapContent
   *  - There are headers and content to display :
   *      m_displayStackHeaders && m_numberOfStacks > 0 && m_contentView
   *  - Either :
   *    - There is more than one header :       m_numberOfStacks > 1
   * The last condition prevents border color mismatch. m_headersOverlapHeaders
   * being false means that the headers' stacks should not share their border.
   * Currently, it only happens in the toolboxes, where the first header stack
   * has a different border color, and should not overlap with anything (second
   * header as well as content). In that case, we ensure that this additional
   * border will not override the first header stack's bottom border. */
  return m_headersOverlapContent && numberOfDisplayedHeaders() > 0 &&
         m_contentView && (m_headersOverlapHeaders || m_numberOfStacks > 1);
}

int StackViewController::ControllerView::numberOfDisplayedHeaders() const {
  unsigned int count = 0;
  for (int i=0; i<m_numberOfStacks; i++) {
    count += isHeaderDisplayed(i);
  }
  return count;
}

void StackViewController::ControllerView::setMaskBit(int i, bool b) {
  assert(i >= 0 && i < kMaxNumberOfStacks);
  if (b) {
    m_displayMask |= (1U << i);
  } else {
    m_displayMask &= ~(1U << i);
  }
}

bool StackViewController::ControllerView::maskBit(int i) const {
  assert(i >= 0 && i < kMaxNumberOfStacks);
  return (m_displayMask >> i) & 1U;
}

int StackViewController::ControllerView::displayedIndex(int i) {
  assert(i < numberOfDisplayedHeaders());
  int index = 0;
  while (index < i) {
    index += isHeaderDisplayed(index);
  }
  return index;
}

int StackViewController::ControllerView::numberOfSubviews() const {
  return numberOfDisplayedHeaders() + (m_contentView == nullptr ? 0 : 1) +
         (borderShouldOverlapContent() ? 1 : 0);
}

View * StackViewController::ControllerView::subviewAtIndex(int index) {
  if (m_displayMask == 0) {
    assert(index == 0);
    return m_contentView;
  }
  int numberOfHeaders = numberOfDisplayedHeaders();
  if (index < numberOfHeaders) {
    assert(index >= 0);
    return &m_stackViews[displayedIndex(index)];
  }
  if (index == numberOfHeaders) {
    return m_contentView;
  } else {
    // Border view must be last so that it is layouted on top of content subview
    assert(index == numberOfHeaders + 1);
    return &m_borderView;
  }
}

#if ESCHER_VIEW_LOGGING
const char * StackViewController::ControllerView::className() const {
  return "StackViewController::ControllerView";
}
#endif

StackViewController::StackViewController(Responder * parentResponder,
                                         ViewController * rootViewController, KDColor textColor,
                                         KDColor backgroundColor, KDColor separatorColor)
    : ViewController(parentResponder), m_view(), m_numberOfChildren(0), m_isVisible(false) {
  pushModel(Frame(rootViewController, textColor, backgroundColor, separatorColor));
  rootViewController->setParentResponder(this);
}

const char * StackViewController::title() {
  ViewController * vc = m_childrenFrame[0].viewController();
  return vc->title();
}

ViewController * StackViewController::topViewController() {
  if (m_numberOfChildren < 1) {
    return nullptr;
  }
  return m_childrenFrame[m_numberOfChildren - 1].viewController();
}

void StackViewController::push(ViewController * vc, KDColor textColor, KDColor backgroundColor,
                               KDColor separatorColor) {
  assert(m_numberOfChildren < kMaxNumberOfStacks);
  Frame frame = Frame(vc, textColor, backgroundColor, separatorColor);
  /* Add the frame to the model */
  pushModel(frame);
  frame.viewController()->initView();
  if (!m_isVisible) {
    return;
  }
  /* Load stack view if the View Controller has a title. */
  if (vc->title() != nullptr &&
      vc->displayParameter() != ViewController::DisplayParameter::DoNotShowOwnTitle) {
    m_view.pushStack(frame);
  }
  setupActiveViewController();
  if (m_numberOfChildren > 1) {
    m_childrenFrame[m_numberOfChildren - 2].viewController()->viewDidDisappear();
  }
}

void StackViewController::pop() {
  assert(m_numberOfChildren > 0);
  ViewController * vc = topViewController();
  if (vc->title() != nullptr &&
      vc->displayParameter() != ViewController::DisplayParameter::DoNotShowOwnTitle) {
    m_view.popStack();
  }
  m_numberOfChildren--;
  setupActiveViewController();
  vc->setParentResponder(nullptr);
  vc->viewDidDisappear();
}

void StackViewController::popUntilDepth(int depth, bool shouldSetupTopViewController) {
  /* If final active view is meant to disappear afterward, there is no need to
   * call setupActiveViewController(), which layouts the final view.
   * For example, shouldSetupTopViewController should be set to false if push,
   * viewDidDisappear, viewWillAppear or pop are called afterward. */
  assert(depth >= 0);
  if (depth >= m_numberOfChildren) {
    return;
  }
  int numberOfFramesReleased = m_numberOfChildren - depth;
  ViewController * vc;
  for (int i = 0; i < numberOfFramesReleased; i++) {
    vc = topViewController();
    if (vc->title() != nullptr &&
        vc->displayParameter() != ViewController::DisplayParameter::DoNotShowOwnTitle) {
      m_view.popStack();
    }
    m_numberOfChildren--;
    if (shouldSetupTopViewController && i == numberOfFramesReleased - 1) {
      setupActiveViewController();
    }
    vc->setParentResponder(nullptr);
    if (i == 0) {
      vc->viewDidDisappear();
    }
  }
}

void StackViewController::pushModel(Frame frame) { m_childrenFrame[m_numberOfChildren++] = frame; }

void StackViewController::setupActiveViewController() {
  ViewController * vc = topViewController();
  if (vc) {
    vc->setParentResponder(this);
    m_view.setDisplayMode(vc->displayParameter());
    m_view.setContentView(vc->view());
    vc->viewWillAppear();
    vc->setParentResponder(this);
    Container::activeApp()->setFirstResponder(vc);
  }
}

void StackViewController::didBecomeFirstResponder() {
  ViewController * vc = topViewController();
  Container::activeApp()->setFirstResponder(vc);
}

bool StackViewController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back && m_numberOfChildren > 1) {
    pop();
    return true;
  }
  return false;
}

void StackViewController::initView() { m_childrenFrame[0].viewController()->initView(); }

void StackViewController::viewWillAppear() {
  /* Load the stack view */
  for (int i = 0; i < m_numberOfChildren; i++) {
    ViewController * childrenVC = m_childrenFrame[i].viewController();
    if (childrenVC->title() != nullptr &&
        childrenVC->displayParameter() != ViewController::DisplayParameter::DoNotShowOwnTitle) {
      m_view.pushStack(m_childrenFrame[i]);
    }
  }
  /* Load the visible controller view */
  ViewController * vc = topViewController();
  if (m_numberOfChildren > 0 && vc) {
    m_view.setContentView(vc->view());
    m_view.setDisplayMode(vc->displayParameter());
    vc->viewWillAppear();
  }
  m_isVisible = true;
}

void StackViewController::viewDidDisappear() {
  ViewController * vc = topViewController();
  if (m_numberOfChildren > 0 && vc) {
    vc->viewDidDisappear();
  }
  int numberOfStacks = m_view.numberOfStacks();
  for (int i = 0; i < numberOfStacks; i++) {
    m_view.popStack();
  }
  m_isVisible = false;
  assert(m_view.numberOfStacks() == 0);
}

}  // namespace Escher
