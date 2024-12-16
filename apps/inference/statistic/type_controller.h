#ifndef INFERENCE_STATISTIC_TYPE_CONTROLLER_H
#define INFERENCE_STATISTIC_TYPE_CONTROLLER_H

#include <escher/chevron_view.h>
#include <escher/highlight_cell.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/selectable_table_view.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>
#include <ion/events.h>

#include "dataset_controller.h"
#include "input_controller.h"
#include "test/hypothesis_controller.h"

namespace Inference {

class TypeController
    : public Escher::UniformSelectableListController<
          Escher::MenuCell<Escher::MessageTextView, Escher::EmptyCellWidget,
                           Escher::ChevronView>,
          3> {
 public:
  TypeController(Escher::StackViewController* parent,
                 HypothesisController* hypothesisController,
                 InputController* intervalInputController,
                 DatasetController* datasetController, Statistic* statistic);
  const char* title() const override;
  ViewController::TitlesDisplay titlesDisplay() const override {
    return ViewController::TitlesDisplay::DisplayLastTitle;
  }
  void stackOpenPage(Escher::ViewController* nextPage) override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;

  constexpr static int k_indexOfTTest = 0;
  constexpr static int k_indexOfPooledTest = 1;
  constexpr static int k_indexOfZTest = 2;

 private:
  HypothesisController* m_hypothesisController;
  InputController* m_inputController;
  DatasetController* m_datasetController;

  constexpr static int k_titleBufferSize =
      sizeof("intervalle pour une moyenne à deux échantillons");
  /* m_titleBuffer is declared as mutable so that ViewController::title() can
   * remain const-qualified in the generic case. */
  mutable char m_titleBuffer[k_titleBufferSize];

  Statistic* m_statistic;
};

}  // namespace Inference

#endif
