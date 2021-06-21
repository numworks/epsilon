#include "test_conclusion_view.h"

#include <apps/i18n.h>
#include <apps/shared/dots.h>
#include <float.h>
#include <kandinsky/font.h>
#include <math.h>

namespace Probability {

void TestConclusionView::Icon::drawRect(KDContext * ctx, KDRect rect) const {
  // TODO replace with actual images
  ctx->fillRect(bounds(), Escher::Palette::WallScreen);
  // Draw circle
  int r = k_size / 2;
  ctx->fillAntialiasedCircle(KDPointZero, r, m_type == Type::Failure ? KDColorRed : KDColorOrange, Palette::WallScreen);

  // Then either checkmark or cross
  if (m_type == Type::Failure) {
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
  m_icon.setFrame(KDRect(KDPoint(k_marginLeft, iconOriginHeight), KDSize(Icon::k_size, Icon::k_size)), force);
  m_textView.setFrame(
      KDRect(KDPoint(Icon::k_size + k_marginBetween + k_marginLeft, 0), KDSize(m_frame.width() - Icon::k_size, m_frame.height())),
      force);
}

KDSize TestConclusionView::minimalSizeForOptimalDisplay() const {
  KDSize iconSize = m_icon.minimalSizeForOptimalDisplay();
  KDSize textSize = m_textView.minimalSizeForOptimalDisplay();
  return KDSize(iconSize.width() + textSize.width(), fmax(iconSize.height(), textSize.height()));
}

}  // namespace Probability
