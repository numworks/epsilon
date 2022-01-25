#include "test_conclusion_view.h"

#include <apps/i18n.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/vertical_offset_layout.h>

#include <algorithm>

#include "../images/test_success.h"
#include "probability/helpers.h"

namespace Probability {

TestConclusionView::TestConclusionView(Statistic * statistic) : m_statistic(statistic) {
  m_textView1.setBackgroundColor(Escher::Palette::WallScreen);
  m_textView1.setAlignment(KDContext::k_alignCenter, KDContext::k_alignCenter);
  m_textView2.setBackgroundColor(Escher::Palette::WallScreen);
  m_textView2.setAlignment(KDContext::k_alignCenter, KDContext::k_alignCenter);
}

void TestConclusionView::drawRect(KDContext * ctx, KDRect rect) const {
  KDSize textsSize = minimalSizeForOptimalDisplay();
  KDCoordinate topOffset = (m_frame.height() - textsSize.height()) / 2;
  ctx->fillRect(KDRect(0, 0, bounds().width(), topOffset), Escher::Palette::WallScreen);
  ctx->fillRect(KDRect(0, bounds().height() - topOffset, bounds().width(), topOffset), Escher::Palette::WallScreen);
}

void TestConclusionView::reload() {
  generateConclusionTexts(m_statistic->canRejectNull());
  markRectAsDirty(bounds());
}

KDSize TestConclusionView::minimalSizeForOptimalDisplay() const {
  KDSize textSize1 = m_textView1.minimalSizeForOptimalDisplay();
  KDSize textSize2 = m_textView2.minimalSizeForOptimalDisplay();
  return KDSize(std::max(textSize1.width(), textSize2.width()), textSize1.height() + textSize2.height());
}

Escher::View * TestConclusionView::subviewAtIndex(int i) {
  assert(i < numberOfSubviews());
  Escher::View * subviews[] = {&m_textView1, &m_textView2};  // Draw icon above text
  return subviews[i];
}

void TestConclusionView::layoutSubviews(bool force) {
  KDSize textsSize = minimalSizeForOptimalDisplay();
  KDCoordinate textHeight = textsSize.height() / 2;
  KDCoordinate topOffset = (m_frame.height() - textsSize.height()) / 2;
  m_textView1.setFrame(KDRect(KDPoint(0, topOffset), KDSize(bounds().width(), textHeight)), force);
  m_textView2.setFrame(KDRect(KDPoint(0, topOffset + textHeight), KDSize(bounds().width(), textHeight)), force);
}

void TestConclusionView::generateConclusionTexts(bool isTestSuccessfull) {
  I18n::Message message;
  Poincare::HorizontalLayout layout = Poincare::HorizontalLayout::Builder(
      Poincare::CodePointLayout::Builder('H'),
      Poincare::VerticalOffsetLayout::Builder(
          Poincare::CodePointLayout::Builder('0'),
          Poincare::VerticalOffsetLayoutNode::Position::Subscript));
  if (isTestSuccessfull) {
    layout.addOrMergeChildAtIndex(
        Poincare::LayoutHelper::String(I18n::translate(I18n::Message::ConclusionSuccess1)),
        2,
        true);
    message = I18n::Message::ConclusionSuccess2;
  } else {
    layout.addOrMergeChildAtIndex(
        Poincare::LayoutHelper::String(I18n::translate(I18n::Message::ConclusionFailure1)),
        2,
        true);
    message = I18n::Message::ConclusionFailure2;
  }
  m_textView1.setLayout(layout);
  m_textView2.setMessage(message);
}

}  // namespace Probability
