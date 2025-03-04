#include "test_controller.h"

#include <apps/apps_container_helper.h>
#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>
#include <poincare/statistics/inference.h>

#include "chi_square/categorical_type_controller.h"
#include "test/hypothesis_controller.h"
#include "type_controller.h"

using namespace Escher;

namespace Inference {

TestController::TestController(StackViewController* parentResponder,
                               HypothesisController* hypothesisController,
                               TypeController* typeController,
                               CategoricalTypeController* categoricalController,
                               InputStoreController* inputStoreController,
                               InputController* inputController,
                               Statistic* statistic)
    : UniformSelectableListController(parentResponder),
      m_hypothesisController(hypothesisController),
      m_typeController(typeController),
      m_inputController(inputController),
      m_categoricalController(categoricalController),
      m_inputStoreController(inputStoreController),
      m_statistic(statistic) {
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
  return I18n::translate(m_statistic->statisticTitle());
}

void TestController::stackOpenPage(ViewController* nextPage) {
  PcrInference::TestType type = m_statistic->testType();
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
  PcrInference::TestType testType;
  int row = selectedRow();
  if (row == k_indexOfOneProp) {
    testType = PcrInference::TestType::OneProportion;
    controller = m_inputController;
    if (m_statistic->hasHypothesisParameters()) {
      controller = m_hypothesisController;
    }
  } else if (row == k_indexOfTwoProps) {
    testType = PcrInference::TestType::TwoProportions;
    controller = m_inputController;
    if (m_statistic->hasHypothesisParameters()) {
      controller = m_hypothesisController;
    }
  } else if (row == k_indexOfOneMean) {
    testType = PcrInference::TestType::OneMean;
    controller = m_typeController;
  } else if (row == k_indexOfTwoMeans) {
    testType = PcrInference::TestType::TwoMeans;
    controller = m_typeController;
  } else if (row == k_indexOfSlope) {
    testType = PcrInference::TestType::Slope;
    controller = m_inputStoreController;
    if (m_statistic->hasHypothesisParameters()) {
      controller = m_hypothesisController;
    }
  } else {
    assert(selectedRow() == k_indexOfChiSquare);
    testType = PcrInference::TestType::Chi2;
    controller = m_categoricalController;
  }
  assert(controller != nullptr);
  if (m_statistic->initializeTest(testType)) {
    controller->selectRow(0);
  }
  stackOpenPage(controller);
  return true;
}

void TestController::viewWillAppear() {
  cell(k_indexOfOneProp)
      ->subLabel()
      ->setMessage(m_statistic->zStatisticMessage());
  cell(k_indexOfOneMean)
      ->subLabel()
      ->setMessage(m_statistic->tOrZStatisticMessage());
  cell(k_indexOfTwoProps)
      ->subLabel()
      ->setMessage(m_statistic->zStatisticMessage());
  cell(k_indexOfTwoMeans)
      ->subLabel()
      ->setMessage(m_statistic->tOrZStatisticMessage());
  cell(k_indexOfChiSquare)
      ->setVisible(m_statistic->numberOfSignificanceTestTypes() ==
                   numberOfRows());
  cell(k_indexOfSlope)
      ->subLabel()
      ->setMessage(m_statistic->tStatisticMessage());
}

}  // namespace Inference
