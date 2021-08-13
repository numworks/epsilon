#include "test_conclusion_view.h"

#include <apps/i18n.h>
#include <apps/shared/dots.h>
#include <float.h>
#include <kandinsky/font.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>

#include <algorithm>
#include <cmath>

#include "../images/test_success.h"
#include "probability/helpers.h"

namespace Probability {

TestConclusionView::TestConclusionView() {
  m_textView1.setBackgroundColor(Escher::Palette::WallScreen);
  m_textView1.setAlignment(0.5f, 0.5f);
  m_textView2.setBackgroundColor(Escher::Palette::WallScreen);
  m_textView2.setAlignment(0.5f, 0.5f);
  m_icon.setImage(ImageStore::TestSuccess);
}

void TestConclusionView::setType(Type t) {
  I18n::Message message;
  Poincare::HorizontalLayout layout = Poincare::HorizontalLayout::Builder(
      Poincare::CodePointLayout::Builder('H'),
      Poincare::VerticalOffsetLayout::Builder(
          Poincare::CodePointLayout::Builder('0'),
          Poincare::VerticalOffsetLayoutNode::Position::Subscript));
  if (t == Type::Success) {
    layout.addOrMergeChildAtIndex(layoutFromText(I18n::translate(I18n::Message::ConclusionSuccess1)),
                                  2,
                                  true);
    message = I18n::Message::ConclusionSuccess2;
  } else {
    layout.addOrMergeChildAtIndex(layoutFromText(I18n::translate(I18n::Message::ConclusionFailure1)),
                                  2,
                                  true);
    message = I18n::Message::ConclusionFailure2;
  }
  m_textView1.setLayout(layout);
  m_textView2.setMessage(message);
  if (t == Type::Success) {
    m_icon.setImage(ImageStore::TestSuccess);
  } else {
    m_icon.setImage(nullptr);
  }
  markRectAsDirty(bounds());
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

}  // namespace Probability
