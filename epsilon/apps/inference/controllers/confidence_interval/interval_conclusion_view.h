#pragma once

#include <escher/buffer_text_view.h>
#include <escher/horizontal_or_vertical_layout.h>
#include <escher/message_text_view.h>

#include "inference/constants.h"

namespace Inference {

class IntervalConclusionView
    : public Escher::MultipleLinesBufferTextView<KDFont::Size::Large, 2> {
 public:
  IntervalConclusionView();
  void setInterval(double center, double marginOfError);
  void reload() { markWholeFrameAsDirty(); }
};

}  // namespace Inference
