extern "C" {
#include <assert.h>
}
#include <escher/container.h>
#include <escher/metric.h>
#include <escher/stack_view_controller.h>

namespace Escher {

StackViewController::ControllerView::ControllerView() :
    View(),
    m_borderView(Palette::GrayBright),
    m_contentView(nullptr),
    m_numberOfStacks(0),
    m_headersDisplayMask(~0),
    m_headersOverlapHeaders(true),
    m_headersOverlapContent(false) {
}

void StackViewController::ControllerView::setContentView(View * view) {
  m_contentView = view;
  layoutSubviews();
  markRectAsDirty(bounds());
}

void StackViewController::ControllerView::setupHeadersBorderOverlaping(
    bool headersOverlapHeaders,
    bool headersOverlapContent,
    KDColor headersContentBorderColor) {
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

bool StackViewController::ControllerView::shouldDisplayHeaderAtIndex(int i) const {
  return maskBit(m_numberOfStacks - 1 - i);
}

void StackViewController::ControllerView::layoutSubviews(bool force) {
  /* Layout:    Overlap   |    No overlap

            |  Header 1   |    Header 1  |
            | ----------  |   ========== |
            |  Header 2   |    Header 2  |
            | ----------  |   ========== |
            |  [content]  |    [content] |
   */
  // Compute view frames
  KDCoordinate width = m_frame.width();
  int heightOffset = 0;
  int heightDiff = Metric::StackTitleHeight +
                   (m_headersOverlapHeaders ? 0 : Metric::CellSeparatorThickness);
  for (int i = 0; i < m_numberOfStacks; i++) {
    if (shouldDisplayHeaderAtIndex(i)) {
      m_stackViews[i].setFrame(
          KDRect(0, heightOffset, width, Metric::StackTitleHeight + Metric::CellSeparatorThickness),
          force);
      heightOffset += heightDiff;
    }
  }
  // Border frame
  if (m_contentView) {
    if (m_headersOverlapHeaders && numberOfDisplayedHeaders() > 0) {
      // Last separator is drawn by last header, so content needs to be offset a bit
      heightOffset += Metric::CellSeparatorThickness;
    }
    if (borderShouldOverlapContent()) {
      // Shift content position up by the separator thickness
      heightOffset -= Metric::CellSeparatorThickness;
      // Layout the common border (which will override content)
      m_borderView.setFrame(KDRect(0, heightOffset, width, Metric::CellSeparatorThickness), force);
    }
    // Content view frame
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
   *      m_displayStackHeaders && numberOfDisplayedHeaders() > 0 && m_contentView
   *  - Either :
   *    - There is more than one header :       m_numberOfStacks > 1
   *    - Or headers overlap each other :       m_headersOverlapHeaders
   * The last condition prevents border color mismatch. m_headersOverlapHeaders
   * being false means that the headers' stacks should not share their border.
   * Currently, it only happens in the toolboxes, where the first header stack
   * has a different border color, and should not overlap with anything (second
   * header as well as content). In that case, we ensure that this additional
   * border will not override the first header stack's bottom border. */
  return m_headersOverlapContent && numberOfDisplayedHeaders() > 0 && m_contentView &&
         (m_headersOverlapHeaders || m_numberOfStacks > 1);
}

int StackViewController::ControllerView::numberOfDisplayedHeaders() const {
  unsigned int count = 0;
  for (int i = 0; i < m_numberOfStacks; i++) {
    count += shouldDisplayHeaderAtIndex(i);
  }
  return count;
}

bool StackViewController::ControllerView::maskBit(int i) const {
  assert(i >= 0 && i < k_MaxNumberOfStacks);
  return (m_headersDisplayMask >> i) & 1U;
}

int StackViewController::ControllerView::displayedIndex(int i) {
  assert(i < numberOfDisplayedHeaders());
  int counted = 0;
  for (int j = 0; j < m_numberOfStacks; j++) {
    if (shouldDisplayHeaderAtIndex(j)) {
      if (counted == i) {
        return j;
      } else {
        counted++;
      }
    }
  }
  assert(false);
  return -1;
}

int StackViewController::ControllerView::numberOfSubviews() const {
  return numberOfDisplayedHeaders() + (m_contentView == nullptr ? 0 : 1) +
         (borderShouldOverlapContent() ? 1 : 0);
}

View * StackViewController::ControllerView::subviewAtIndex(int index) {
  if (m_headersDisplayMask == 0) {
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
  }
  // Border view must be last so that it is layouted on top of content subview
  assert(index == numberOfHeaders + 1);
  return &m_borderView;
}

#if ESCHER_VIEW_LOGGING
const char * StackViewController::ControllerView::className() const {
  return "StackViewController::ControllerView";
}
#endif

StackViewController::StackViewController(Responder * parentResponder,
                                         ViewController * rootViewController,
                                         KDColor textColor,
                                         KDColor backgroundColor,
                                         KDColor separatorColor) :
    ViewController(parentResponder), m_view(), m_numberOfChildren(0), m_isVisible(false) {
  pushModel(Frame(rootViewController, textColor, backgroundColor, separatorColor));
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

void StackViewController::push(ViewController * vc,
                               KDColor textColor,
                               KDColor backgroundColor,
                               KDColor separatorColor) {
  assert(m_numberOfChildren < k_MaxNumberOfStacks);
  Frame frame = Frame(vc, textColor, backgroundColor, separatorColor);
  /* Add the frame to the model */
  pushModel(frame);
  frame.viewController()->initView();
  if (!m_isVisible) {
    return;
  }
  /* Load stack view if the View Controller has a title. */
  if (shouldStoreHeaderOnStack(vc)) {
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
  if (shouldStoreHeaderOnStack(vc)) {
    /* Warning: this assumes that the DisplayParameter and title return the
     * same as when they were pushed on the stack. */
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
    if (shouldStoreHeaderOnStack(vc)) {
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

void StackViewController::pushModel(Frame frame) {
  m_childrenFrame[m_numberOfChildren++] = frame;
}

void StackViewController::setupActiveView() {
  ViewController * vc = topViewController();
  if (vc) {
    ViewController::DisplayParameter topHeaderDisplayParameter = vc->displayParameter();
    // If NeverDisplayOwnTitle, we show all other headers -> DisplayAllTitles
    ViewController::DisplayParameter displayMask =
        topHeaderDisplayParameter == ViewController::DisplayParameter::NeverDisplayOwnTitle
            ? ViewController::DisplayParameter::DisplayAllTitles
            : topHeaderDisplayParameter;
    m_view.setHeadersDisplayMask(displayMask);
    m_view.setContentView(vc->view());
    vc->viewWillAppear();
  }
}

void StackViewController::setupActiveViewController() {
  setupActiveView();
  ViewController * vc = topViewController();
  if (vc) {
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

void StackViewController::initView() {
  m_childrenFrame[0].viewController()->initView();
}

void StackViewController::viewWillAppear() {
  /* Load the stack view */
  for (int i = 0; i < m_numberOfChildren; i++) {
    ViewController * childrenVC = m_childrenFrame[i].viewController();
    if (shouldStoreHeaderOnStack(childrenVC)) {
      m_view.pushStack(m_childrenFrame[i]);
    }
  }
  /* Load the visible controller view */
  setupActiveView();
  m_isVisible = true;
}

void StackViewController::viewDidDisappear() {
  ViewController * vc = topViewController();
  if (vc) {
    vc->viewDidDisappear();
  }
  int numberOfStacks = m_view.numberOfStacks();
  for (int i = 0; i < numberOfStacks; i++) {
    m_view.popStack();
  }
  m_isVisible = false;
  assert(m_view.numberOfStacks() == 0);
}

bool StackViewController::shouldStoreHeaderOnStack(ViewController * vc) {
  /* In general, the displayParameter controls how the stack is shown
   * only while the controller is the last on the stack.
   * However, with NeverDisplayOwnTitle, the title will never be pushed
   * onto the stack and therefore never shown, even in sub-controllers. */
  return vc->title() != nullptr &&
         vc->displayParameter() != ViewController::DisplayParameter::NeverDisplayOwnTitle;
}

}  // namespace Escher
