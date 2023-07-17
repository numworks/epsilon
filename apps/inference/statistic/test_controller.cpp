#include "test_controller.h"

#include <apps/apps_container_helper.h>
#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>

#include "chi_square_and_slope/categorical_type_controller.h"
#include "inference/app.h"
#include "test/hypothesis_controller.h"
#include "type_controller.h"

using namespace Escher;

namespace Inference {

TestController::TestController(StackViewController *parentResponder,
                               HypothesisController *hypothesisController,
                               TypeController *typeController,
                               CategoricalTypeController *categoricalController,
                               InputSlopeController *inputSlopeController,
                               InputController *inputController,
                               Statistic *statistic)
    : SelectableCellListPage(parentResponder),
      m_hypothesisController(hypothesisController),
      m_typeController(typeController),
      m_inputController(inputController),
      m_categoricalController(categoricalController),
      m_inputSlopeController(inputSlopeController),
      m_statistic(statistic) {
  typedCell(k_indexOfOneProp)->label()->setMessage(I18n::Message::TestOneProp);
  typedCell(k_indexOfOneMean)->label()->setMessage(I18n::Message::TestOneMean);
  typedCell(k_indexOfTwoProps)
      ->label()
      ->setMessage(I18n::Message::TestTwoProps);
  typedCell(k_indexOfTwoMeans)
      ->label()
      ->setMessage(I18n::Message::TestTwoMeans);
  typedCell(k_indexOfCategorical)
      ->label()
      ->setMessage(I18n::Message::TestCategorical);
  typedCell(k_indexOfCategorical)
      ->subLabel()
      ->setMessage(I18n::Message::X2Test);
  typedCell(k_indexOfSlope)->label()->setMessage(I18n::Message::Slope);
  // Init selection
  selectRow(0);
}

const char *TestController::title() {
  return I18n::translate(m_statistic->statisticTitle());
}

void TestController::stackOpenPage(ViewController *nextPage) {
  SignificanceTestType type = m_statistic->significanceTestType();
  selectRow(static_cast<int>(type));
  ViewController::stackOpenPage(nextPage);
}

void TestController::didBecomeFirstResponder() {
  resetSizeMemoization();
  m_selectableListView.reloadData();
  ExplicitSelectableListViewController::didBecomeFirstResponder();
}

bool TestController::handleEvent(Ion::Events::Event event) {
  // canBeActivatedByEvent can be called on any cell with chevron
  if (!typedCell(0)->canBeActivatedByEvent(event)) {
    return popFromStackViewControllerOnLeftEvent(event);
  }
  SelectableViewController *controller = nullptr;
  SignificanceTestType testType;
  int row = selectedRow();
  if (row == k_indexOfOneProp) {
    testType = SignificanceTestType::OneProportion;
    controller = m_inputController;
    if (m_statistic->hasHypothesisParameters()) {
      controller = m_hypothesisController;
    }
  } else if (row == k_indexOfTwoProps) {
    testType = SignificanceTestType::TwoProportions;
    controller = m_inputController;
    if (m_statistic->hasHypothesisParameters()) {
      controller = m_hypothesisController;
    }
  } else if (row == k_indexOfOneMean) {
    testType = SignificanceTestType::OneMean;
    controller = m_typeController;
  } else if (row == k_indexOfTwoMeans) {
    testType = SignificanceTestType::TwoMeans;
    controller = m_typeController;
  } else if (row == k_indexOfSlope) {
    testType = SignificanceTestType::Slope;
    controller = m_inputSlopeController;
    if (m_statistic->hasHypothesisParameters()) {
      controller = m_hypothesisController;
    }
  } else {
    assert(selectedRow() == k_indexOfCategorical);
    testType = SignificanceTestType::Categorical;
    controller = m_categoricalController;
  }
  assert(controller != nullptr);
  if (m_statistic->initializeSignificanceTest(
          testType, AppsContainerHelper::sharedAppsContainerGlobalContext())) {
    controller->selectRow(0);
  }
  stackOpenPage(controller);
  return true;
}

void TestController::viewWillAppear() {
  typedCell(k_indexOfOneProp)
      ->subLabel()
      ->setMessage(m_statistic->zStatisticMessage());
  typedCell(k_indexOfOneMean)
      ->subLabel()
      ->setMessage(m_statistic->tOrZStatisticMessage());
  typedCell(k_indexOfTwoProps)
      ->subLabel()
      ->setMessage(m_statistic->zStatisticMessage());
  typedCell(k_indexOfTwoMeans)
      ->subLabel()
      ->setMessage(m_statistic->tOrZStatisticMessage());
  typedCell(k_indexOfCategorical)
      ->setVisible(m_statistic->numberOfSignificancesTestTypes() ==
                   numberOfRows());
  typedCell(k_indexOfCategorical)
      ->subLabel()
      ->setMessage(m_statistic->tStatisticMessage());
}

}  // namespace Inference
