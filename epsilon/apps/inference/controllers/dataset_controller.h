#pragma once

#include <escher/chevron_view.h>
#include <escher/menu_cell.h>
#include <escher/selectable_list_view_controller.h>
#include <inference/controllers/input_controller.h>
#include <inference/controllers/store/input_store_controller.h>
#include <inference/models/input_table_from_store.h>

#include "inference_controller.h"

namespace Inference {

class DatasetControllerCell
    : public Escher::MenuCell<Escher::MessageTextView, Escher::EmptyCellWidget,
                              Escher::ChevronView> {
 public:
  using Escher::MenuCell<Escher::MessageTextView, Escher::EmptyCellWidget,
                         Escher::ChevronView>::MenuCell;
  /* DatasetController is a UniformSelectableListController that doesn't allow
   * fillCellForRow override. We can't set the message on constructor since they
   * are changed on initView, which is called after the cells have been laid
   * out. As a result, we allow the cells to be re-laid out after changing the
   * label here. This exception could be removed by reworking the entire
   * DatasetController structure, at the cost of some optimisations. */
  void updateLabelAndReload(I18n::Message message);
};

class DatasetController : public InferenceController,
                          public Escher::UniformSelectableListController<
                              DatasetControllerCell,
                              InputTableFromStore::k_numberOfDatasetOptions> {
 public:
  DatasetController(Escher::StackViewController* parent,
                    ControllerContainer* controllerContainer,
                    InferenceModel* inference);

  void initView() override;

  const char* title() const override {
    InputController::InputTitle(this, m_inferenceModel, m_titleBuffer,
                                InputController::k_titleBufferSize);
    return m_titleBuffer;
  }
  TitlesDisplay titlesDisplay() const override {
    return m_inferenceModel->hasHypothesisParameters()
               ? TitlesDisplay::DisplayLastTwoTitles
               : TitlesDisplay::DisplayLastTitle;
  }
  bool handleEvent(Ion::Events::Event) override;

 private:
  enum class Options {
    InputStatisticsAndInputDataset,  // For One-Mean
    InputDataAndInputStatistics      // For ANOVA
  };

  Options getListOptions() const;

  int indexOfInputStatisticsCell() const;
  int indexOfDatasetCell() const;
  int indexOfInputDataCell() const;

  /* m_titleBuffer is declared as mutable so that ViewController::title() can
   * remain const-qualified in the generic case. */
  mutable char m_titleBuffer[InputController::k_titleBufferSize];
};

}  // namespace Inference
