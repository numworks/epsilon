#ifndef PROBABILITY_GUI_INTERVAL_CONCLUSION_VIEW_H
#define PROBABILITY_GUI_INTERVAL_CONCLUSION_VIEW_H

#include <escher/buffer_text_view.h>
#include <escher/message_text_view.h>

#include "probability/constants.h"
#include <escher/horizontal_or_vertical_layout.h>

namespace Probability {

class IntervalConclusionView : public Escher::BufferTextView {
public:
  IntervalConclusionView();
  void setInterval(double center, double marginOfError);
};

}  // namespace Probability

#endif /* PROBABILITY_GUI_INTERVAL_CONCLUSION_VIEW_H */
