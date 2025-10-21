#include "test_controller.h"

#include <apps/apps_container_helper.h>
#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>
#include <poincare/statistics/inference.h>

#include "controller_container.h"
#include "inference/controllers/inference_controller.h"
#include "inference/models/aliases.h"

using namespace Escher;

namespace Inference {

TestController::TestController(StackViewController* parentResponder,
                               ControllerContainer* controllerContainer,
                               InferenceModel* inference)
    : InferenceController(inference, controllerContainer),
      UniformSelectableListController(parentResponder) {
  cell(k_indexOfOneProp)->label()->setMessage(I18n::Message::OneProportion);
  cell(k_indexOfOneMean)->label()->setMessage(I18n::Message::OneMean);
  cell(k_indexOfTwoProps)->label()->setMessage(I18n::Message::TwoProportions);
  cell(k_indexOfTwoMeans)->label()->setMessage(I18n::Message::TwoMeans);
  cell(k_indexOfChiSquare)->label()->setMessage(I18n::Message::ChiSquare);
  cell(k_indexOfChiSquare)->subLabel()->setMessage(I18n::Message::Chi2Test);
  cell(k_indexOfANOVA)->label()->setMessage(I18n::Message::ANOVAOneWay);
  cell(k_indexOfSlope)->label()->setMessage(I18n::Message::Slope);
  // Init selection
  selectRow(0);
}

const char* TestController::title() const {
  return I18n::translate(m_inferenceModel->subAppTitle());
}

void TestController::stackOpenPage(ViewController* nextPage) {
  TestType type = m_inferenceModel->testType();
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
  } else if (row == k_indexOfANOVA) {
    testType = TestType::ANOVA;
  } else if (row == k_indexOfSlope) {
    testType = TestType::Slope;
  } else {
    assert(selectedRow() == k_indexOfChiSquare);
    testType = TestType::Chi2;
  }
  bool statChanged = m_inferenceModel->initializeTest(testType);
  if (m_inferenceModel->testType() == TestType::Chi2) {
    controller = &m_controllerContainer->m_categoricalTypeController;
  } else if (m_inferenceModel->numberOfAvailableStatistics() > 1) {
    controller = &m_controllerContainer->m_typeController;
  } else if (m_inferenceModel->hasHypothesisParameters()) {
    if (m_inferenceModel->testType() == TestType::ANOVA) {
      controller = &m_controllerContainer->m_hypothesisDisplayOnlyController;
    } else {
      controller = &m_controllerContainer->m_hypothesisEditableController;
    }
  } else if (m_inferenceModel->testType() == TestType::Slope) {
    controller = &m_controllerContainer->m_inputStoreController1;
  } else {
    controller = &m_controllerContainer->m_inputController;
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
      ->setMessage(m_inferenceModel->zStatisticMessage());
  cell(k_indexOfOneMean)
      ->subLabel()
      ->setMessage(m_inferenceModel->tOrZStatisticMessage());
  cell(k_indexOfTwoProps)
      ->subLabel()
      ->setMessage(m_inferenceModel->zStatisticMessage());
  cell(k_indexOfTwoMeans)
      ->subLabel()
      ->setMessage(m_inferenceModel->tOrZStatisticMessage());
  /* Chi-square and ANOVA are only available in the Test sub-app, not in the
   * Interval sub-app */
  cell(k_indexOfChiSquare)
      ->setVisible(m_inferenceModel->subApp() == SubApp::SignificanceTest);
  cell(k_indexOfANOVA)
      ->setVisible(m_inferenceModel->subApp() == SubApp::SignificanceTest);
  cell(k_indexOfANOVA)->subLabel()->setMessage(I18n::Message::FTest);
  cell(k_indexOfSlope)
      ->subLabel()
      ->setMessage(m_inferenceModel->tStatisticMessage());
}

}  // namespace Inference
