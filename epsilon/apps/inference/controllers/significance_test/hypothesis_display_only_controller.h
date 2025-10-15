#pragma once

#include <escher/button_cell.h>
#include <escher/layout_view.h>
#include <escher/menu_cell.h>

#include "hypothesis_controller.h"
#include "inference/controllers/dataset_controller.h"
#include "inference/controllers/input_controller.h"
#include "inference/controllers/store/input_store_controller.h"

namespace Inference {

class HypothesisDisplayOnlyController : public HypothesisController {
 public:
  HypothesisDisplayOnlyController(Escher::StackViewController* parent,
                                  InputController* inputController,
                                  InputStoreController* inputStoreController,
                                  DatasetController* datasetController,
                                  SignificanceTest* test);

  const Escher::HighlightCell* cell(int i) const override;

  using CellType = Escher::MenuCell<Escher::LayoutView, Escher::MessageTextView,
                                    Escher::BufferTextView<10>>;

 private:
  CellType m_h0;
  CellType m_ha;
};

}  // namespace Inference
