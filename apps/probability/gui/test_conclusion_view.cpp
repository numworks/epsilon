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
  drawCircle(ctx, rect, KDPointZero, r, m_type == Type::Failure ? KDColorRed : KDColorOrange);

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

void TestConclusionView::Icon::drawCircle(KDContext * ctx, KDRect rect, KDPoint center, int radius,
                                          KDColor color, KDColor background) const {
  constexpr static int maxD = 30;
  assert(radius < maxD);
  float buffer[maxD * maxD]{0};
  KDColor colorBuffer[maxD * maxD];
  int maxI = std::round((radius)*M_SQRT1_2);
  int r2 = radius * radius;
  float hi, hip1 = radius - 1e-5, alphaA, alphaB;
  int qi;
  for (int i = 0; i < maxI; i++) {
    hi = hip1;
    hip1 = std::sqrt(r2 - pow(i + 1, 2));
    qi = std::floor(hi - 1e-5);
    if (qi > hip1) {
      alphaA = (hi - qi) / 2;
      alphaB = (2 + hip1 - qi) / 2;
      setToAllSymetries(buffer, alphaB, i, qi - 1, radius);
      setToAllSymetries(buffer, alphaA, i, qi, radius);
    } else {
      // Only one pixel
      alphaB = (hi + hip1 - 2 * qi) / 2;
      setToAllSymetries(buffer, alphaB, i, qi, radius);
    }

    // Filled lines
    for (int j = qi - 1; j > -qi - 1; j--) {
      buffer[posToIndex(i, j, radius)] = 1;
      buffer[posToIndex(-i - 1, j, radius)] = 1;
    }
    for (int j = i; j > -i - 2; j--) {
      buffer[posToIndex(qi - 1, j, radius)] = 1;
      buffer[posToIndex(-qi, j, radius)] = 1;
    }
  }
  // Convert to KDColor array
  uint8_t r = color.red(), g = color.green(), b = color.blue();
  uint8_t br = background.red(), bg = background.green(), bb = background.blue();
  for (int index = 0; index < (2 * radius) * (2 * radius); index++) {
    float alpha = buffer[index];
    colorBuffer[index] = KDColor::RGB888(r * alpha + br * (1 - alpha), g * alpha + bg * (1 - alpha),
                                         b * alpha + bb * (1 - alpha));
  }
  KDRect circleRect = KDRect(rect.origin().translatedBy(center), KDSize(2 * radius, 2 * radius));
  ctx->fillRectWithPixels(circleRect, colorBuffer, colorBuffer);
}

int TestConclusionView::Icon::posToIndex(int i, int j, int r) {
  return r + i + (r - 1 - j) * 2 * r;
}

void TestConclusionView::Icon::setToAllSymetries(float buffer[], float alpha, int i, int j, int r) {
  buffer[posToIndex(i, j, r)] = alpha;
  buffer[posToIndex(i, -j - 1, r)] = alpha;
  buffer[posToIndex(-i - 1, j, r)] = alpha;
  buffer[posToIndex(-i - 1, -j - 1, r)] = alpha;
  buffer[posToIndex(j, i, r)] = alpha;
  buffer[posToIndex(-j - 1, i, r)] = alpha;
  buffer[posToIndex(j, -i - 1, r)] = alpha;
  buffer[posToIndex(-j - 1, -i - 1, r)] = alpha;
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
