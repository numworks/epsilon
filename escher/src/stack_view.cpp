#include <escher/stack_view.h>
#include <escher/palette.h>
#include <escher/metric.h>

namespace Escher {

StackView::StackView(Style style, bool extendVertically) :
    View(),
    m_borderView(Palette::GrayBright),
    m_contentView(nullptr),
    m_style(style),
    m_headersOverlapHeaders(true),
    m_headersOverlapContent(false),
    m_extendVertically(extendVertically)
{
}

void StackView::setContentView(View * view) {
  m_contentView = view;
  layoutSubviews();
  markRectAsDirty(bounds());
}

void StackView::setupHeadersBorderOverlaping(
    bool headersOverlapHeaders,
    bool headersOverlapContent,
    KDColor headersContentBorderColor) {
  m_headersOverlapHeaders = headersOverlapHeaders;
  m_headersOverlapContent = headersOverlapContent;
  m_borderView.setColor(headersContentBorderColor);
}

void StackView::pushStack(ViewController * vc) {
  KDColor textColor = Palette::GrayDarkMiddle;
  KDColor backgroundColor = KDColorWhite;
  KDColor separatorColor = Palette::GrayBright;
  int numberOfStacks = m_stackViewHeaders.length();
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
  m_stackViewHeaders.push(StackViewHeader(vc, textColor, backgroundColor, separatorColor));
}

KDSize StackView::minimalSizeForOptimalDisplay() const {
  if (m_contentView == nullptr) {
    return KDSizeZero;
  }
  KDSize size = m_contentView->minimalSizeForOptimalDisplay();
  int heightDiff = Metric::StackTitleHeight + (m_headersOverlapHeaders ? 0 : Metric::CellSeparatorThickness);
  int numberOfStacks = m_stackViewHeaders.length();
  assert(m_extendVertically || numberOfStacks > 0);
  return KDSize(size.width(), m_extendVertically ? 0 : (size.height() + heightDiff * numberOfStacks + Metric::CellSeparatorThickness));
}

void StackView::layoutSubviews(bool force) {
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
  int numberOfStacks = m_stackViewHeaders.length();
  for (int i = 0; i < numberOfStacks; i++) {
    m_stackViewHeaders.elementAtIndex(i)->setFrame(
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

bool StackView::borderShouldOverlapContent() const {
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
  int numberOfStacks = m_stackViewHeaders.length();
  return m_headersOverlapContent && numberOfStacks > 0 && m_contentView &&
         (m_headersOverlapHeaders || numberOfStacks > 1);
}

int StackView::numberOfSubviews() const {
  return m_stackViewHeaders.length() + (m_contentView == nullptr ? 0 : 1) +
         (borderShouldOverlapContent() ? 1 : 0);
}

View * StackView::subviewAtIndex(int index) {
  int numberOfStacks = m_stackViewHeaders.length();
  if (index < numberOfStacks) {
    assert(index >= 0);
    return m_stackViewHeaders.elementAtIndex(index);
  }
  if (index == numberOfStacks) {
    return m_contentView;
  }
  // Border view must be last so that it is layouted on top of content subview
  assert(index == numberOfStacks + 1);
  return &m_borderView;
}

#if ESCHER_VIEW_LOGGING
const char * StackView::className() const {
  return "StackView";
}
#endif

}  // namespace Escher
