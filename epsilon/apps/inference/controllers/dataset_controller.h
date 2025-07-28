#ifndef INFERENCE_STATISTIC_DATASET_CONTROLLER_H
#define INFERENCE_STATISTIC_DATASET_CONTROLLER_H

#include <escher/chevron_view.h>
#include <escher/menu_cell.h>
#include <escher/selectable_list_view_controller.h>
#include <inference/controllers/input_controller.h>
#include <inference/controllers/store/input_store_controller.h>
#include <inference/models/input_table_from_store.h>

namespace Inference {

class DatasetController
    : public Escher::UniformSelectableListController<
          Escher::MenuCell<Escher::MessageTextView, Escher::EmptyCellWidget,
                           Escher::ChevronView>,
          OneMeanStatistic::k_numberOfDatasetOptions> {
 public:
  DatasetController(Escher::StackViewController* parent,
                    InputController* inputController,
                    InputStoreController* storeController,
                    InferenceModel* inference);

  const char* title() const override {
    InputController::InputTitle(this, m_inference, m_titleBuffer,
                                InputController::k_titleBufferSize);
    return m_titleBuffer;
  }
  TitlesDisplay titlesDisplay() const override {
    return m_inference->hasHypothesisParameters()
               ? TitlesDisplay::DisplayLastTwoTitles
               : TitlesDisplay::DisplayLastTitle;
  }
  bool handleEvent(Ion::Events::Event) override;

 private:
  constexpr static int k_indexOfInputStatisticsCell = 0;
  constexpr static int k_indexOfDatasetCell = 1;

  InputController* m_inputController;
  InputStoreController* m_storeController;
  InferenceModel* m_inference;

  /* m_titleBuffer is declared as mutable so that ViewController::title() can
   * remain const-qualified in the generic case. */
  mutable char m_titleBuffer[InputController::k_titleBufferSize];
};

}  // namespace Inference

#endif
