#ifndef APPS_PROBABILITY_GUI_INTERVAL_CONCLUSION_VIEW_H
#define APPS_PROBABILITY_GUI_INTERVAL_CONCLUSION_VIEW_H

#include <escher/buffer_text_view.h>
#include <escher/message_text_view.h>

#include "probability/constants.h"
#include "probability/gui/horizontal_or_vertical_layout.h"

namespace Probability {

class IntervalConclusionView : public VerticalLayout {
public:
  IntervalConclusionView();
  void setInterval(float center, float marginOfError);
  int numberOfSubviews() const override { return 2; }
  Escher::View * subviewAtIndex(int i) override;

private:
  constexpr static int k_intervalBufferSize = Constants::k_shortFloatNumberOfChars + sizeof("±") +
                                              Constants::k_shortFloatNumberOfChars + 1 /* \0 */;
  Escher::MessageTextView m_messageView;
  Escher::BufferTextView m_intervalView;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_INTERVAL_CONCLUSION_VIEW_H */
