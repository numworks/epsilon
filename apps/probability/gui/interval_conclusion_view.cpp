#include "interval_conclusion_view.h"

#include <apps/i18n.h>
#include <kandinsky/font.h>

#include "probability/text_helpers.h"

namespace Probability {

IntervalConclusionView::IntervalConclusionView() {
  setFont(KDFont::LargeFont);
  setAlignment(KDContext::k_alignCenter, KDContext::k_alignCenter);
  setBackgroundColor(Escher::Palette::WallScreen);
}

void IntervalConclusionView::setInterval(float center, float marginOfError) {
  constexpr static int bufferSize = Constants::k_shortBufferSize;
  char bufferCenter[bufferSize];
  char buffermarginOfError[bufferSize];

  defaultConvertFloatToText(center, bufferCenter, bufferSize);
  defaultConvertFloatToText(marginOfError, buffermarginOfError, bufferSize);

  snprintf(m_buffer,
           k_maxNumberOfChar,
           "%s\n%s ± %s",
           I18n::translate(I18n::Message::ConfidenceInterval),
           bufferCenter,
           buffermarginOfError);
}

}  // namespace Probability
