#ifndef APPS_PROBABILITY_GUI_INTERVAL_CONCLUSION_VIEW_H
#define APPS_PROBABILITY_GUI_INTERVAL_CONCLUSION_VIEW_H

#include <escher/buffer_text_view.h>
#include <escher/message_text_view.h>

#include "probability/constants.h"
#include "probability/gui/horizontal_or_vertical_layout.h"

namespace Probability {

class IntervalConclusionView : public Escher::BufferTextView {
public:
  IntervalConclusionView();
  void setInterval(float center, float marginOfError);
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_INTERVAL_CONCLUSION_VIEW_H */
