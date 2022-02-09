extern "C" {
#include <assert.h>
}
#include <escher/container.h>
#include <escher/metric.h>
#include <escher/stack_view_controller.h>
#include <ion/bit_helper.h>

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
  return Ion::BitHelper::bitAtIndex(m_headersDisplayMask, m_numberOfStacks - 1 - i);
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

int StackViewController::ControllerView::stackHeaderIndex(int displayIndex) {
  assert(displayIndex < numberOfDisplayedHeaders());
  int counted = 0;
  for (int j = 0; j < m_numberOfStacks; j++) {
    if (shouldDisplayHeaderAtIndex(j)) {
      if (counted == displayIndex) {
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
    return &m_stackViews[stackHeaderIndex(index)];
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

StackViewController::StackViewController(Responder * parentResponder, ViewController * rootViewController, Style style, int styleStep) :
    ViewController(parentResponder), m_view(), m_numberOfChildren(0), m_isVisible(false) {
  pushModel(Frame(rootViewController, style, styleStep, m_numberOfChildren));
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

StackViewController::Frame::Frame(ViewController * viewController, Style style, int styleStep, uint8_t numberOfChildren) :
    m_viewController(viewController) {
  if (style == Style::WhiteUniform) {
    m_textColor = Palette::GrayDarkMiddle;
    m_backgroundColor = KDColorWhite;
    m_separatorColor = Palette::GrayBright;
  } else {
    assert(style == Style::GrayGradation);
    m_textColor = KDColorWhite;
    if (numberOfChildren == 0 && styleStep < 0) {
      m_backgroundColor = Palette::PopUpTitleBackground;
      m_separatorColor = Palette::PurpleDark;
    } else {
      constexpr KDColor k_grayGradationColors[] = { Palette::PurpleBright, Palette::GrayDark, Palette::GrayDarkMiddle};
      styleStep = styleStep < 0 ? numberOfChildren - 1 : styleStep;
      assert(styleStep >= 0 && styleStep < sizeof(k_grayGradationColors)/sizeof(KDColor));
      m_backgroundColor = k_grayGradationColors[styleStep];
      m_separatorColor = k_grayGradationColors[styleStep];
    }
  }
}

void StackViewController::push(ViewController * vc, Style style, int styleStep) {
  assert(m_numberOfChildren < k_MaxNumberOfStacks);
  Frame frame = Frame(vc, style, styleStep, m_numberOfChildren);
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
    /* Warning: this assumes that the TitlesDisplay and title return the
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
    ViewController::TitlesDisplay topHeaderDisplayParameter = vc->titlesDisplay();
    // If NeverDisplayOwnTitle, we show all other headers -> DisplayAllTitles
    ViewController::TitlesDisplay displayMask =
        topHeaderDisplayParameter == ViewController::TitlesDisplay::NeverDisplayOwnTitle
            ? ViewController::TitlesDisplay::DisplayAllTitles
            : topHeaderDisplayParameter;
    m_view.setHeadersDisplayMask(displayMask);
    m_view.setContentView(vc->view());
    vc->viewWillAppear();
  }
}

void StackViewController::setupActiveViewController() {
  ViewController * vc = topViewController();
  if (vc) {
    vc->setParentResponder(this);
  }
  setupActiveView();
  if (vc) {
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
  /* In general, the titlesDisplay controls how the stack is shown
   * only while the controller is the last on the stack.
   * However, with NeverDisplayOwnTitle, the title will never be pushed
   * onto the stack and therefore never shown, even in sub-controllers. */
  return vc->title() != nullptr &&
         vc->titlesDisplay() != ViewController::TitlesDisplay::NeverDisplayOwnTitle;
}

}  // namespace Escher
