extern "C" {
#include <assert.h>
}
#include <escher/container.h>
#include <escher/metric.h>
#include <escher/stack_view_controller.h>
#include <ion/bit_helper.h>

namespace Escher {

StackViewController::ControllerView::ControllerView(Style style, bool extendVertically) :
    View(),
    m_borderView(Palette::GrayBright),
    m_contentView(nullptr),
    m_style(style),
    m_headersOverlapHeaders(true),
    m_headersOverlapContent(false),
    m_extendVertically(extendVertically)
{
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

void StackViewController::ControllerView::pushStack(ViewController * vc) {
  KDColor textColor = Palette::GrayDarkMiddle;
  KDColor backgroundColor = KDColorWhite;
  KDColor separatorColor = Palette::GrayBright;
  int numberOfStacks = m_stackViews.length();
  if (m_style == Style::GrayGradation) {
    textColor = KDColorWhite;
    constexpr KDColor k_grayGradationColors[] = { Palette::PurpleBright, Palette::GrayDark, Palette::GrayDarkMiddle};
    backgroundColor = k_grayGradationColors[numberOfStacks];
    separatorColor = k_grayGradationColors[numberOfStacks];
  } else if (m_style == Style::PurpleWhite) {
    if (numberOfStacks == 0) {
      textColor = KDColorWhite;
      backgroundColor = Palette::PopUpTitleBackground;
      separatorColor = Palette::PurpleDark;
    }
  } else {
    assert(m_style == Style::WhiteUniform);
  }
  m_stackViews.push(StackView(vc, textColor, backgroundColor, separatorColor));
}

KDSize StackViewController::ControllerView::minimalSizeForOptimalDisplay() const {
  if (m_contentView == nullptr) {
    return KDSizeZero;
  }
  KDSize size = m_contentView->minimalSizeForOptimalDisplay();
  int heightDiff = Metric::StackTitleHeight + (m_headersOverlapHeaders ? 0 : Metric::CellSeparatorThickness);
  int numberOfStacks = m_stackViews.length();
  assert(m_extendVertically || numberOfStacks > 0);
  return KDSize(size.width(), m_extendVertically ? 0 : (size.height() + heightDiff * numberOfStacks + Metric::CellSeparatorThickness));
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
  int heightDiff = Metric::StackTitleHeight + (m_headersOverlapHeaders ? 0 : Metric::CellSeparatorThickness);
  int numberOfStacks = m_stackViews.length();
  for (int i = 0; i < numberOfStacks; i++) {
    m_stackViews.elementAtIndex(i)->setFrame(
        KDRect(0, heightOffset, width, Metric::StackTitleHeight + Metric::CellSeparatorThickness),
        force);
    heightOffset += heightDiff;
  }
  // Border frame
  if (m_contentView) {
    if (m_headersOverlapHeaders && numberOfStacks > 0) {
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
   *      m_displayStackHeaders && numberOfStacks > 0 && m_contentView
   *  - Either :
   *    - There is more than one header :       numberOfStacks > 1
   *    - Or headers overlap each other :       m_headersOverlapHeaders
   * The last condition prevents border color mismatch. m_headersOverlapHeaders
   * being false means that the headers' stacks should not share their border.
   * Currently, it only happens in the toolboxes, where the first header stack
   * has a different border color, and should not overlap with anything (second
   * header as well as content). In that case, we ensure that this additional
   * border will not override the first header stack's bottom border. */
  int numberOfStacks = m_stackViews.length();
  return m_headersOverlapContent && numberOfStacks > 0 && m_contentView &&
         (m_headersOverlapHeaders || numberOfStacks > 1);
}

int StackViewController::ControllerView::numberOfSubviews() const {
  return m_stackViews.length() + (m_contentView == nullptr ? 0 : 1) +
         (borderShouldOverlapContent() ? 1 : 0);
}

View * StackViewController::ControllerView::subviewAtIndex(int index) {
  int numberOfStacks = m_stackViews.length();
  if (index < numberOfStacks) {
    assert(index >= 0);
    return m_stackViews.elementAtIndex(index);
  }
  if (index == numberOfStacks) {
    return m_contentView;
  }
  // Border view must be last so that it is layouted on top of content subview
  assert(index == numberOfStacks + 1);
  return &m_borderView;
}

#if ESCHER_VIEW_LOGGING
const char * StackViewController::ControllerView::className() const {
  return "StackViewController::ControllerView";
}
#endif

StackViewController::StackViewController(Responder * parentResponder, ViewController * rootViewController, Style style, bool extendVertically) :
    ViewController(parentResponder),
    m_view(style, extendVertically),
    m_numberOfChildren(0),
    m_isVisible(false),
    m_headersDisplayMask(~0)
{
  m_childrenController[m_numberOfChildren++] = rootViewController;
  rootViewController->setParentResponder(this);
}

const char * StackViewController::title() {
  ViewController * vc = m_childrenController[0];
  return vc->title();
}

ViewController * StackViewController::topViewController() {
  if (m_numberOfChildren < 1) {
    return nullptr;
  }
  return m_childrenController[m_numberOfChildren - 1];
}

void StackViewController::push(ViewController * vc) {
  assert(m_numberOfChildren < k_MaxNumberOfStacks);
  /* Add the frame to the model */
  pushModel(vc);
  vc->initView();
  if (!m_isVisible) {
    return;
  }
  setupActiveViewController();
  if (m_numberOfChildren > 1) {
    m_childrenController[m_numberOfChildren - 2]->viewDidDisappear();
  }
}

void StackViewController::pop() {
  assert(m_numberOfChildren > 0);
  dismissPotentialModal();
  ViewController * vc = topViewController();
  m_numberOfChildren--;
  setupActiveViewController();
  vc->setParentResponder(nullptr);
  vc->viewDidDisappear();
  didExitPage(vc);
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
  dismissPotentialModal();
  int numberOfFramesReleased = m_numberOfChildren - depth;
  ViewController * vc;
  for (int i = 0; i < numberOfFramesReleased; i++) {
    vc = topViewController();
    m_numberOfChildren--;
    if (shouldSetupTopViewController && i == numberOfFramesReleased - 1) {
      setupActiveViewController();
    }
    vc->setParentResponder(nullptr);
    if (i == 0) {
      vc->viewDidDisappear();
    }
    didExitPage(vc);
  }
}

void StackViewController::pushModel(ViewController * controller) {
  willOpenPage(controller);
  m_childrenController[m_numberOfChildren++] = controller;
}

void StackViewController::setupActiveView() {
  ViewController * vc = topViewController();
  if (vc) {
    ViewController::TitlesDisplay topHeaderDisplayParameter = vc->titlesDisplay();
    updateStack(topHeaderDisplayParameter);
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
  m_childrenController[0]->initView();
}

void StackViewController::viewWillAppear() {
  /* Load the visible controller view */
  setupActiveView();
  m_isVisible = true;
}

void StackViewController::viewDidDisappear() {
  ViewController * vc = topViewController();
  if (vc) {
    vc->viewDidDisappear();
  }
  m_isVisible = false;
  m_view.m_contentView = nullptr;
}

bool StackViewController::shouldStoreHeaderOnStack(ViewController * vc, int index) {
  /* In general, the titlesDisplay controls how the stack is shown
   * only while the controller is the last on the stack. */
  return vc->title() != nullptr &&
         vc->titlesDisplay() != ViewController::TitlesDisplay::NeverDisplayOwnTitle &&
         Ion::BitHelper::bitAtIndex(m_headersDisplayMask, m_numberOfChildren - 1 - index);
}

void StackViewController::updateStack(ViewController::TitlesDisplay titleDisplay) {
  /* Update the header display mask */
  // If NeverDisplayOwnTitle, we show all other headers -> DisplayAllTitles
  m_headersDisplayMask = static_cast<uint8_t>(titleDisplay);

  /* Load the stack view */
  m_view.resetStack();
  for (int i = 0; i < m_numberOfChildren; i++) {
    ViewController * childrenVC = m_childrenController[i];
    if (shouldStoreHeaderOnStack(childrenVC, i)) {
      m_view.pushStack(m_childrenController[i]);
    }
  }
}

void StackViewController::didExitPage(ViewController * controller) const {
  Container::activeApp()->didExitPage(controller);
}

void StackViewController::willOpenPage(ViewController * controller) const {
  Container::activeApp()->willOpenPage(controller);
}

void StackViewController::dismissPotentialModal() {
  Container::activeApp()->modalViewController()->dismissPotentialModal();
}

}  // namespace Escher
