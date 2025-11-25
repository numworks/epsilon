#pragma once

#include <escher/chevron_view.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <inference/models/inference_model.h>
#include <ion/events.h>

#include "inference_controller.h"

namespace Inference {

class ControllerContainer;

constexpr static int k_numberOfTests = 7;

class TestController
    : public InferenceController,
      public Escher::UniformSelectableListController<
          Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                           Escher::ChevronView>,
          k_numberOfTests> {
 public:
  TestController(Escher::StackViewController* parentResponder,
                 ControllerContainer* controllerContainer,
                 InferenceModel* inference);
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
};

}  // namespace Inference
