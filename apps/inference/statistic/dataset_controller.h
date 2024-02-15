#ifndef INFERENCE_STATISTIC_DATASET_CONTROLLER_H
#define INFERENCE_STATISTIC_DATASET_CONTROLLER_H

#include <escher/chevron_view.h>
#include <escher/menu_cell.h>
#include <escher/selectable_list_view_controller.h>
#include <inference/models/statistic/one_mean_statistic.h>
#include <inference/statistic/input_controller.h>

namespace Inference {

class DatasetController
    : public Escher::UniformSelectableListController<
          Escher::MenuCell<Escher::MessageTextView, Escher::EmptyCellWidget,
                           Escher::ChevronView>,
          OneMeanStatistic::k_numberOfDatasetOptions> {
 public:
  DatasetController(Escher::StackViewController* parent,
                    InputController* inputController, Statistic* statistic)
      : UniformSelectableListController(parent),
        m_inputController(inputController),
        m_statistic(statistic) {
    cell(k_indexOfInputStatisticsCell)
        ->label()
        ->setMessage(I18n::Message::InputStatistics);
  }

  const char* title() override { return ""; }  // TODO
  TitlesDisplay titlesDisplay() override {
    return TitlesDisplay::DisplayAllTitles;
  }  // TODO
  bool handleEvent(Ion::Events::Event) override;

 private:
  constexpr static int k_indexOfInputStatisticsCell = 0;

  InputController* m_inputController;
  Statistic* m_statistic;
};

}  // namespace Inference

#endif
