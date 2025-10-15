#pragma once

#include <escher/chevron_view.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <inference/models/inference_model.h>
#include <ion/events.h>

#include "significance_test/hypothesis_display_only_controller.h"
#include "significance_test/hypothesis_editable_controller.h"

namespace Inference {

class HypothesisEditableController;
class CategoricalTypeController;
class TypeController;
class InputController;
class InputStoreController;

static constexpr int k_numberOfTests = 7;

class TestController
    : public Escher::UniformSelectableListController<
          Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                           Escher::ChevronView>,
          k_numberOfTests> {
 public:
  TestController(
      Escher::StackViewController* parentResponder,
      HypothesisEditableController* hypothesisEditableController,
      HypothesisDisplayOnlyController* HypothesisDisplayOnlyController,
      TypeController* typeController,
      CategoricalTypeController* categoricalController,
      InputStoreController* inputStoreController,
      InputController* inputController, InferenceModel* inference);
  void stackOpenPage(Escher::ViewController* nextPage) override;
  bool handleEvent(Ion::Events::Event e) override;
  const char* title() const override;
  void viewWillAppear() override;

  constexpr static int k_indexOfOneProp = 0;
  constexpr static int k_indexOfOneMean = 1;
  constexpr static int k_indexOfTwoProps = 2;
  constexpr static int k_indexOfTwoMeans = 3;
  constexpr static int k_indexOfChiSquare = 4;
  constexpr static int k_indexOfANOVA = 5;
  constexpr static int k_indexOfSlope = 6;

 protected:
  void handleResponderChainEvent(ResponderChainEvent event) override;

 private:
  HypothesisEditableController* m_hypothesisEditableController;
  HypothesisDisplayOnlyController* m_HypothesisDisplayOnlyController;
  TypeController* m_typeController;
  InputController* m_inputController;
  CategoricalTypeController* m_categoricalController;
  InputStoreController* m_inputStoreController;
  InferenceModel* m_inference;
};

}  // namespace Inference
