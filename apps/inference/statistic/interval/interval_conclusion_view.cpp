#include "interval_conclusion_view.h"

#include <apps/i18n.h>
#include <kandinsky/font.h>
#include <poincare/old/print.h>
#include <poincare/preferences.h>

#include "inference/text_helpers.h"

namespace Inference {

IntervalConclusionView::IntervalConclusionView() {
  setFont(KDFont::Size::Large);
  setAlignment(KDGlyph::k_alignCenter, KDGlyph::k_alignCenter);
  setBackgroundColor(Escher::Palette::WallScreen);
}

void IntervalConclusionView::setInterval(double center, double marginOfError) {
  Poincare::Print::CustomPrintf(
      m_buffer, maxTextSize(), "%s\n%*.*ed ± %*.*ed",
      I18n::translate(I18n::Message::ConfidenceInterval), center,
      Poincare::Preferences::PrintFloatMode::Decimal,
      Poincare::Preferences::LargeNumberOfSignificantDigits, marginOfError,
      Poincare::Preferences::PrintFloatMode::Decimal,
      Poincare::Preferences::LargeNumberOfSignificantDigits);
}

}  // namespace Inference
