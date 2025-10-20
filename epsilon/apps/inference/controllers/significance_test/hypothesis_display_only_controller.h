#pragma once

#include <escher/buffer_text_view.h>
#include <escher/button_cell.h>
#include <escher/layout_view.h>
#include <escher/menu_cell.h>

#include "hypothesis_controller.h"

namespace Inference {

class ControllerContainer;

class HypothesisDisplayOnlyController : public HypothesisController {
 public:
  HypothesisDisplayOnlyController(Responder* parent,
                                  ControllerContainer* controllerContainer,
                                  SignificanceTest* test);

  const Escher::HighlightCell* cell(int i) const override;

  using CellType = Escher::MenuCell<Escher::LayoutView, Escher::MessageTextView,
                                    Escher::BufferTextView<10>>;

 protected:
  void handleResponderChainEvent(ResponderChainEvent event) override;

 private:
  CellType m_h0;
  CellType m_ha;
};

}  // namespace Inference
