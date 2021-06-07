#include "test_conclusion_view.h"

#include <apps/i18n.h>
#include <apps/shared/dots.h>
#include <math.h>

namespace Probability {

void TestConclusionView::Icon::drawRect(KDContext * ctx, KDRect rect) const {
  // TODO replace with actual images
  // Draw circle
  float middle = k_size / 2, width;
  int start, end;
  for (float i = 0; i < k_size; i++) {
    float x = 2 * (i - middle) / k_size;
    width = k_size / 2 * sqrt(1 - pow(x, 2));
    start = middle - width;
    end = middle + width;
    ctx->drawLine(KDPoint(i, middle - width), KDPoint(i, middle + width), KDColorBlack);
  }
  // Then either checkmark or cross
  if (m_type == Type::Success) {
    KDPoint p1 = KDPoint(k_size / 4, k_size / 4);
    KDPoint p2 = KDPoint(3 * k_size / 4 + 1, 3 * k_size / 4 + 1);
    KDPoint p3 = KDPoint(k_size / 4, 3 * k_size / 4);
    KDPoint p4 = KDPoint(3 * k_size / 4 + 1, k_size / 4 - 1);
    ctx->drawLine(p1, p2, KDColorWhite);
    ctx->drawLine(p3, p4, KDColorWhite);
  } else {
    KDPoint p1 = KDPoint(k_size / 3, k_size / 2);
    KDPoint p2 = KDPoint(k_size / 2, 3 * k_size / 4);
    KDPoint p3 = KDPoint(3 * k_size / 3, k_size / 4);
    ctx->drawLine(p1, p2, KDColorWhite);
    ctx->drawLine(p2, p3, KDColorWhite);
  }
}

KDSize TestConclusionView::Icon::minimalSizeForOptimalDisplay() const {
  return KDSize(k_size, k_size);
}

void TestConclusionView::setType(Type t) {
  I18n::Message m =
      t == Type::Success ? I18n::Message::ConclusionSuccess : I18n::Message::ConclusionFailure;
  m_textView.setMessage(m);
  m_icon.setType(t);
  markRectAsDirty(bounds());
}

Escher::View * TestConclusionView::subviewAtIndex(int i) {
  assert(i < numberOfSubviews());
  Escher::View * subviews[] = {&m_icon, &m_textView};
  return subviews[i];
}

void TestConclusionView::layoutSubviews(bool force) {
  int iconOriginHeight = (m_frame.height() - Icon::k_size) / 2;
  m_icon.setFrame(KDRect(KDPoint(0, iconOriginHeight), KDSize(Icon::k_size, Icon::k_size)), force);
  m_textView.setFrame(
      KDRect(KDPoint(Icon::k_size + k_margin, 0), KDSize(m_frame.width() - Icon::k_size, m_frame.height())),
      force);
}

KDSize TestConclusionView::minimalSizeForOptimalDisplay() const {
  KDSize iconSize = m_icon.minimalSizeForOptimalDisplay();
  KDSize textSize = m_textView.minimalSizeForOptimalDisplay();
  return KDSize(iconSize.width() + textSize.width(), fmax(iconSize.height(), textSize.height()));
}

}  // namespace Probability
