#include "test_conclusion_view.h"

#include <apps/i18n.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>

#include <algorithm>

#include "../images/test_success.h"
#include "probability/helpers.h"

namespace Probability {

TestConclusionView::TestConclusionView(Statistic * statistic) : m_statistic(statistic) {
  m_textView1.setBackgroundColor(Escher::Palette::WallScreen);
  m_textView1.setAlignment(KDFont::ALIGN_CENTER, KDFont::ALIGN_CENTER);
  m_textView2.setBackgroundColor(Escher::Palette::WallScreen);
  m_textView2.setAlignment(KDFont::ALIGN_CENTER, KDFont::ALIGN_CENTER);
  m_icon.setImage(ImageStore::TestSuccess);
}

Escher::View * TestConclusionView::subviewAtIndex(int i) {
  assert(i < numberOfSubviews());
  Escher::View * subviews[] = {&m_textView1, &m_textView2, &m_icon};  // Draw icon above text
  return subviews[i];
}

void TestConclusionView::layoutSubviews(bool force) {
  int iconOriginHeight = (m_frame.height() - k_iconSize) / 2;
  m_icon.setFrame(
      KDRect(KDPoint(k_marginLeft, iconOriginHeight), m_icon.minimalSizeForOptimalDisplay()),
      force);
  KDSize textSize1 = m_textView1.minimalSizeForOptimalDisplay();
  KDSize textSize2 = m_textView2.minimalSizeForOptimalDisplay();
  KDCoordinate textHeight = textSize1.height() + textSize2.height();
  KDCoordinate topOffset = (m_frame.height() - textHeight) / 2;
  m_textView1.setFrame(KDRect(KDPoint(0, topOffset), KDSize(m_frame.width(), textSize1.height())),
                       force);
  m_textView2.setFrame(KDRect(KDPoint(0, topOffset + textSize1.height()),
                              KDSize(m_frame.width(), textSize2.height())),
                       force);
}

KDSize TestConclusionView::minimalSizeForOptimalDisplay() const {
  KDSize iconSize = m_icon.minimalSizeForOptimalDisplay();
  KDSize textSize1 = m_textView1.minimalSizeForOptimalDisplay();
  KDSize textSize2 = m_textView2.minimalSizeForOptimalDisplay();
  return KDSize(iconSize.width() + std::max(textSize1.width(), textSize2.width()),
                std::max<int>(iconSize.height(), textSize1.height() + textSize2.height()));
}

void TestConclusionView::drawRect(KDContext * ctx, KDRect rect) const {
  KDSize contentSize = minimalSizeForOptimalDisplay();
  KDCoordinate topOffset = (m_frame.height() - contentSize.height()) / 2;
  ctx->fillRect(KDRect(0, 0, bounds().width(), topOffset), Escher::Palette::WallScreen);
  ctx->fillRect(KDRect(0, bounds().height() - topOffset, bounds().width(), topOffset),
                Escher::Palette::WallScreen);
}

void TestConclusionView::reload() {
  generateConclusionTexts(m_statistic->canRejectNull());
  m_icon.setImage(m_statistic->canRejectNull() ? ImageStore::TestSuccess : nullptr);
  markRectAsDirty(bounds());
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
        layoutFromText(I18n::translate(I18n::Message::ConclusionSuccess1)),
        2,
        true);
    message = I18n::Message::ConclusionSuccess2;
  } else {
    layout.addOrMergeChildAtIndex(
        layoutFromText(I18n::translate(I18n::Message::ConclusionFailure1)),
        2,
        true);
    message = I18n::Message::ConclusionFailure2;
  }
  m_textView1.setLayout(layout);
  m_textView2.setMessage(message);
}

}  // namespace Probability
