#include "interval_conclusion_view.h"

#include <apps/i18n.h>
#include <poincare/preferences.h>
#include <poincare/print_float.h>
#include <kandinsky/font.h>

#include "probability/text_helpers.h"

namespace Probability {

IntervalConclusionView::IntervalConclusionView() {
  m_messageView.setMessage(I18n::Message::ConfidenceInterval);
  m_messageView.setAlignment(0.5, 0.5);
  m_intervalView.setFont(KDFont::LargeFont);
  m_intervalView.setAlignment(0.5, 0.5);
}

void IntervalConclusionView::setInterval(float center, float ME) {
  constexpr static int bufferSize = 20;
  char bufferCenter[bufferSize];
  char bufferME[bufferSize];
  Poincare::PrintFloat::ConvertFloatToText(center, bufferCenter, bufferSize, 10, 3,
                                           Poincare::Preferences::PrintFloatMode::Decimal);
  Poincare::PrintFloat::ConvertFloatToText(ME, bufferME, bufferSize, 10, 3,
                                           Poincare::Preferences::PrintFloatMode::Decimal);

  char buffer[bufferSize];
  sprintf(buffer, "%s +/- %s", bufferCenter, bufferME);
  m_intervalView.setText(buffer);
  layoutSubviews();
}

Escher::View * IntervalConclusionView::subviewAtIndex(int i) {
  assert(i < 2);
  if (i == 0) {
    return &m_messageView;
  }
  return &m_intervalView;
}

}  // namespace Probability
