#include "interval_conclusion_view.h"

#include <apps/i18n.h>
#include <kandinsky/font.h>
#include <poincare/preferences.h>
#include <poincare/print.h>

#include "inference/text_helpers.h"

namespace Inference {

IntervalConclusionView::IntervalConclusionView() {
  setFont(KDFont::Size::Large);
  setAlignment(KDGlyph::k_alignCenter, KDGlyph::k_alignCenter);
  setBackgroundColor(Escher::Palette::WallScreen);
}

void IntervalConclusionView::setInterval(double center, double marginOfError) {
  constexpr static int bufferSize = Constants::k_shortBufferSize;
  char bufferCenter[bufferSize];
  char buffermarginOfError[bufferSize];

  defaultConvertFloatToText(center, bufferCenter, bufferSize);
  defaultConvertFloatToText(marginOfError, buffermarginOfError, bufferSize);
  // Use the same precision as in StatisticCurveView::drawInterval
  Poincare::Print::CustomPrintf(
      m_buffer, k_maxNumberOfChar, "%s\n%*.*ed ± %*.*ed",
      I18n::translate(I18n::Message::ConfidenceInterval), center,
      Poincare::Preferences::PrintFloatMode::Decimal,
      Poincare::Preferences::LargeNumberOfSignificantDigits, marginOfError,
      Poincare::Preferences::PrintFloatMode::Decimal,
      Poincare::Preferences::LargeNumberOfSignificantDigits);
}

}  // namespace Inference
