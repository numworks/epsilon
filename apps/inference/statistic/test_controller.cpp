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
    : SelectableListViewController<MemoizedListViewDataSource>(parentResponder),
      m_hypothesisController(hypothesisController),
      m_typeController(typeController),
      m_inputController(inputController),
      m_categoricalController(categoricalController),
      m_inputSlopeController(inputSlopeController),
      m_statistic(statistic) {
  // Init selection
  selectRow(0);
}

const char *TestController::title() {
  return I18n::translate(m_statistic->statisticTitle());
}

void TestController::stackOpenPage(ViewController *nextPage) {
  SignificanceTestType type = m_statistic->significanceTestType();
  selectRow(type == SignificanceTestType::Slope ? virtualIndexOfSlope()
                                                : static_cast<int>(type));
  ViewController::stackOpenPage(nextPage);
}

void TestController::didBecomeFirstResponder() {
  SelectableListViewController<
      MemoizedListViewDataSource>::didBecomeFirstResponder();
  resetMemoization();
  m_selectableListView.reloadData();
}

bool TestController::handleEvent(Ion::Events::Event event) {
  // canBeActivatedByEvent can be called on any cell with chevron
  if (!m_cells[0].canBeActivatedByEvent(event)) {
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
  } else if (row == virtualIndexOfSlope()) {
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

int TestController::numberOfRows() const {
  return m_statistic->numberOfSignificancesTestTypes();
}

void TestController::willDisplayCellForIndex(HighlightCell *cell, int index) {
  MenuCell<MessageTextView, MessageTextView, ChevronView> *c =
      static_cast<MenuCell<MessageTextView, MessageTextView, ChevronView> *>(
          cell);
  if (index == virtualIndexOfSlope()) {
    c->label()->setMessage(I18n::Message::Slope);
    c->subLabel()->setMessage(m_statistic->tStatisticMessage());
    return;
  }
  switch (index) {
    case k_indexOfOneProp:
      c->label()->setMessage(I18n::Message::TestOneProp);
      c->subLabel()->setMessage(m_statistic->zStatisticMessage());
      return;
    case k_indexOfOneMean:
      c->label()->setMessage(I18n::Message::TestOneMean);
      c->subLabel()->setMessage(m_statistic->tOrZStatisticMessage());
      return;
    case k_indexOfTwoProps:
      c->label()->setMessage(I18n::Message::TestTwoProps);
      c->subLabel()->setMessage(m_statistic->zStatisticMessage());
      return;
    case k_indexOfTwoMeans:
      c->label()->setMessage(I18n::Message::TestTwoMeans);
      c->subLabel()->setMessage(m_statistic->tOrZStatisticMessage());
      return;
    default:
      assert(index == k_indexOfCategorical);
      c->label()->setMessage(I18n::Message::TestCategorical);
      c->subLabel()->setMessage(I18n::Message::X2Test);
      return;
  }
}

}  // namespace Inference
