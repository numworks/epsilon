#include "test_controller.h"

#include <apps/apps_container_helper.h>
#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>
#include <poincare/statistics/inference.h>

#include "chi_square/categorical_type_controller.h"
#include "significance_test/hypothesis_controller.h"
#include "type_controller.h"

using namespace Escher;

namespace Inference {

TestController::TestController(StackViewController* parentResponder,
                               HypothesisController* hypothesisController,
                               TypeController* typeController,
                               CategoricalTypeController* categoricalController,
                               InputStoreController* inputStoreController,
                               InputController* inputController,
                               InferenceModel* inference)
    : UniformSelectableListController(parentResponder),
      m_hypothesisController(hypothesisController),
      m_typeController(typeController),
      m_inputController(inputController),
      m_categoricalController(categoricalController),
      m_inputStoreController(inputStoreController),
      m_inference(inference) {
  cell(k_indexOfOneProp)->label()->setMessage(I18n::Message::OneProportion);
  cell(k_indexOfOneMean)->label()->setMessage(I18n::Message::OneMean);
  cell(k_indexOfTwoProps)->label()->setMessage(I18n::Message::TwoProportions);
  cell(k_indexOfTwoMeans)->label()->setMessage(I18n::Message::TwoMeans);
  cell(k_indexOfChiSquare)->label()->setMessage(I18n::Message::ChiSquare);
  cell(k_indexOfChiSquare)->subLabel()->setMessage(I18n::Message::Chi2Test);
  cell(k_indexOfSlope)->label()->setMessage(I18n::Message::Slope);
  // Init selection
  selectRow(0);
}

const char* TestController::title() const {
  return I18n::translate(m_inference->subAppTitle());
}

void TestController::stackOpenPage(ViewController* nextPage) {
  TestType type = m_inference->testType();
  selectRow(static_cast<int>(type));
  ViewController::stackOpenPage(nextPage);
}

void TestController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    m_selectableListView.reloadData();
    ExplicitSelectableListViewController::handleResponderChainEvent(event);
  } else {
    UniformSelectableListController::handleResponderChainEvent(event);
  }
}

bool TestController::handleEvent(Ion::Events::Event event) {
  // canBeActivatedByEvent can be called on any cell with chevron
  if (!cell(0)->canBeActivatedByEvent(event)) {
    return popFromStackViewControllerOnLeftEvent(event);
  }
  SelectableViewController* controller = nullptr;
  TestType testType;
  int row = selectedRow();
  if (row == k_indexOfOneProp) {
    testType = TestType::OneProportion;
  } else if (row == k_indexOfTwoProps) {
    testType = TestType::TwoProportions;
  } else if (row == k_indexOfOneMean) {
    testType = TestType::OneMean;
  } else if (row == k_indexOfTwoMeans) {
    testType = TestType::TwoMeans;
  } else if (row == k_indexOfSlope) {
    testType = TestType::Slope;
  } else {
    assert(selectedRow() == k_indexOfChiSquare);
    testType = TestType::Chi2;
  }
  bool statChanged = m_inference->initializeTest(testType);
  if (m_inference->testType() == TestType::Chi2) {
    controller = m_categoricalController;
  } else if (m_inference->numberOfAvailableStatistics() > 1) {
    controller = m_typeController;
  } else if (m_inference->hasHypothesisParameters()) {
    controller = m_hypothesisController;
  } else if (m_inference->testType() == TestType::Slope) {
    controller = m_inputStoreController;
  } else {
    controller = m_inputController;
  }
  if (statChanged) {
    controller->selectRow(0);
  }
  stackOpenPage(controller);
  return true;
}

void TestController::viewWillAppear() {
  cell(k_indexOfOneProp)
      ->subLabel()
      ->setMessage(m_inference->zStatisticMessage());
  cell(k_indexOfOneMean)
      ->subLabel()
      ->setMessage(m_inference->tOrZStatisticMessage());
  cell(k_indexOfTwoProps)
      ->subLabel()
      ->setMessage(m_inference->zStatisticMessage());
  cell(k_indexOfTwoMeans)
      ->subLabel()
      ->setMessage(m_inference->tOrZStatisticMessage());
  cell(k_indexOfChiSquare)
      ->setVisible(m_inference->numberOfTestTypes() == numberOfRows());
  cell(k_indexOfSlope)
      ->subLabel()
      ->setMessage(m_inference->tStatisticMessage());
}

}  // namespace Inference
