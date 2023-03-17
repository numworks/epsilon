#ifndef INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_CATEGORICAL_TYPE_CONTROLLER_H
#define INFERENCE_STATISTIC_CHI_SQUARE_AND_SLOPE_CATEGORICAL_TYPE_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/chevron_view.h>
#include <escher/highlight_cell.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <ion/events.h>

#include "inference/models/statistic/chi2_test.h"

namespace Inference {

class InputGoodnessController;
class InputHomogeneityController;

constexpr static int k_numberOfCategoricalCells = 2;

class CategoricalTypeController
    : public Escher::SelectableCellListPage<
          Escher::MenuCell<Escher::MessageTextView, Escher::EmptyCellWidget,
                           Escher::ChevronView>,
          k_numberOfCategoricalCells, Escher::RegularListViewDataSource> {
 public:
  CategoricalTypeController(
      Escher::StackViewController* parent, Chi2Test* statistic,
      InputGoodnessController* inputGoodnessController,
      InputHomogeneityController* inputHomogeneityController);
  ViewController::TitlesDisplay titlesDisplay() override {
    return ViewController::TitlesDisplay::DisplayLastTitle;
  }
  const char* title() override {
    return I18n::translate(I18n::Message::CategoricalTypeControllerTitle);
  }
  void stackOpenPage(Escher::ViewController* nextPage) override;
  bool handleEvent(Ion::Events::Event event) override;

  constexpr static int k_indexOfGoodnessCell = 0;
  constexpr static int k_indexOfHomogeneityCell = 1;

 private:
  Chi2Test* m_statistic;
  InputGoodnessController* m_inputGoodnessController;
  InputHomogeneityController* m_inputHomogeneityController;
};

}  // namespace Inference
#endif
