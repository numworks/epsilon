#include "test_controller.h"

#include <apps/apps_container_helper.h>
#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>
#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>

#include "chi_square_and_slope/categorical_type_controller.h"
#include "inference/app.h"
#include "test/hypothesis_controller.h"
#include "type_controller.h"

using namespace Inference;

TestController::TestController(Escher::StackViewController *parentResponder,
                               HypothesisController *hypothesisController,
                               TypeController *typeController,
                               CategoricalTypeController *categoricalController,
                               InputSlopeController *inputSlopeController,
                               InputController *inputController,
                               Statistic *statistic)
    : Escher::SelectableListViewController<Escher::MemoizedListViewDataSource>(
          parentResponder),
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

void TestController::stackOpenPage(Escher::ViewController *nextPage) {
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
  if (event == Ion::Events::OK || event == Ion::Events::EXE ||
      event == Ion::Events::Right) {
    Escher::SelectableViewController *controller = nullptr;
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
            testType,
            AppsContainerHelper::sharedAppsContainerGlobalContext())) {
      controller->selectRow(0);
    }
    stackOpenPage(controller);
    return true;
  }
  return popFromStackViewControllerOnLeftEvent(event);
}

int TestController::numberOfRows() const {
  return m_statistic->numberOfSignificancesTestTypes();
}

void TestController::willDisplayCellForIndex(Escher::HighlightCell *cell,
                                             int index) {
  Escher::MessageTableCellWithChevronAndMessage *c =
      static_cast<Escher::MessageTableCellWithChevronAndMessage *>(cell);
  if (index == virtualIndexOfSlope()) {
    c->setMessage(I18n::Message::Slope);
    c->setSubtitle(m_statistic->tStatisticMessage());
    return;
  }
  switch (index) {
    case k_indexOfOneProp:
      c->setMessage(I18n::Message::TestOneProp);
      c->setSubtitle(m_statistic->zStatisticMessage());
      return;
    case k_indexOfOneMean:
      c->setMessage(I18n::Message::TestOneMean);
      c->setSubtitle(m_statistic->tOrZStatisticMessage());
      return;
    case k_indexOfTwoProps:
      c->setMessage(I18n::Message::TestTwoProps);
      c->setSubtitle(m_statistic->zStatisticMessage());
      return;
    case k_indexOfTwoMeans:
      c->setMessage(I18n::Message::TestTwoMeans);
      c->setSubtitle(m_statistic->tOrZStatisticMessage());
      return;
    default:
      assert(index == k_indexOfCategorical);
      c->setMessage(I18n::Message::TestCategorical);
      c->setSubtitle(I18n::Message::X2Test);
      return;
  }
}
