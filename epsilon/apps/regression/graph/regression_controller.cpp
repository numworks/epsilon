#include "regression_controller.h"

#include <apps/shared/interactive_curve_view_controller.h>
#include <assert.h>
#include <escher/container.h>
#include <escher/stack_view_controller.h>

#include <algorithm>

#include "../model.h"

using namespace Poincare;
using namespace Escher;

namespace Regression {

RegressionController::RegressionController(Responder* parentResponder,
                                           Store* store)
    : SelectableListViewController(parentResponder),
      m_store(store),
      m_series(-1),
      m_displayedFromDataTab(true) {}

const char* RegressionController::title() const {
  if (displaySeriesNameAsTitle()) {
    return Store::SeriesTitle(m_series);
  }
  return I18n::translate(I18n::Message::RegressionModel);
}

ViewController::TitlesDisplay RegressionController::titlesDisplay() const {
  if (displaySeriesNameAsTitle()) {
    return ViewController::TitlesDisplay::DisplayLastTitle;
  }
  return ViewController::TitlesDisplay::DisplayLastTwoTitles;
}

void RegressionController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    Model::Type type = m_store->seriesRegressionType(m_series);
    int initialIndex = std::max(0, IndexOfModelType(type));
    if (initialIndex >= numberOfRows()) {
      assert(type == Model::Type::LinearApbx &&
             GlobalPreferences::SharedGlobalPreferences()
                     ->regressionAppVariant() ==
                 CountryPreferences::RegressionApp::Default);
      // Type is hidden for selected country, select the first line.
      initialIndex = 0;
    } else {
      assert(!Store::HasCoefficients(type) ||
             type == ModelTypeAtIndex(initialIndex));
    }
    m_selectableListView.selectCell(initialIndex);
    SelectableListViewController<
        MemoizedListViewDataSource>::handleResponderChainEvent(event);
  } else {
    SelectableListViewController<
        MemoizedListViewDataSource>::handleResponderChainEvent(event);
  }
}

bool RegressionController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    assert(m_series > -1);
    m_store->setSeriesRegressionType(m_series, ModelTypeAtIndex(selectedRow()));
    StackViewController* stack =
        static_cast<StackViewController*>(parentResponder());
    stack->popUntilDepth(
        Shared::InteractiveCurveViewController::k_graphControllerStackDepth,
        true);
    return true;
  }
  if (event == Ion::Events::Left) {
    StackViewController* stack =
        static_cast<StackViewController*>(parentResponder());
    stack->pop();
    return true;
  }
  return false;
}
KDCoordinate RegressionController::nonMemoizedRowHeight(int row) {
  assert(row >= 0 && row < numberOfRows());
  MenuCell<MessageTextView, LayoutView> tempCell;
  return protectedNonMemoizedRowHeight(&tempCell, row);
}

HighlightCell* RegressionController::reusableCell(int index, int type) {
  assert(index >= 0);
  assert(index < k_numberOfCells);
  return &m_regressionCells[index];
}

void RegressionController::fillCellForRow(HighlightCell* cell, int row) {
  assert(row >= 0 && row < numberOfRows());
  MenuCell<MessageTextView, LayoutView>* myCell =
      static_cast<MenuCell<MessageTextView, LayoutView>*>(cell);
  Model* model = m_store->regressionModel(ModelTypeAtIndex(row));
  myCell->label()->setMessage(model->name());
  myCell->subLabel()->setLayout(model->templateLayout());
}

}  // namespace Regression
