#ifndef INFERENCE_STATISTIC_TEST_CONTROLLER_H
#define INFERENCE_STATISTIC_TEST_CONTROLLER_H

#include <escher/chevron_view.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <ion/events.h>

#include "inference/models/statistic/statistic.h"

namespace Inference {

class HypothesisController;
class CategoricalTypeController;
class TypeController;
class InputController;
class InputStoreController;

class TestController
    : public Escher::UniformSelectableListController<
          Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                           Escher::ChevronView>,
          Statistic::k_numberOfSignificanceTestType> {
 public:
  TestController(Escher::StackViewController* parentResponder,
                 HypothesisController* hypothesisController,
                 TypeController* typeController,
                 CategoricalTypeController* categoricalController,
                 InputStoreController* inputStoreController,
                 InputController* inputController, Statistic* statistic);
  void stackOpenPage(Escher::ViewController* nextPage) override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event e) override;
  const char* title() const override;
  void viewWillAppear() override;

  constexpr static int k_indexOfOneProp = 0;
  constexpr static int k_indexOfOneMean = 1;
  constexpr static int k_indexOfTwoProps = 2;
  constexpr static int k_indexOfTwoMeans = 3;
  constexpr static int k_indexOfChiSquare = 4;
  constexpr static int k_indexOfSlope = 5;

 private:
  HypothesisController* m_hypothesisController;
  TypeController* m_typeController;
  InputController* m_inputController;
  CategoricalTypeController* m_categoricalController;
  InputStoreController* m_inputStoreController;
  Statistic* m_statistic;
};

}  // namespace Inference

#endif
