#include "regression_controller.h"

#include <apps/shared/interactive_curve_view_controller.h>
#include <assert.h>
#include <escher/container.h>
#include <escher/stack_view_controller.h>

#include <algorithm>

#include "../model/cubic_model.h"
#include "../model/exponential_model.h"
#include "../model/linear_model.h"
#include "../model/logarithmic_model.h"
#include "../model/logistic_model.h"
#include "../model/median_model.h"
#include "../model/power_model.h"
#include "../model/quadratic_model.h"
#include "../model/quartic_model.h"
#include "../model/trigonometric_model.h"

using namespace Poincare;
using namespace Escher;

namespace Regression {

RegressionController::RegressionController(Responder *parentResponder,
                                           Store *store)
    : SelectableListViewController(parentResponder),
      m_store(store),
      m_series(-1),
      m_displayedFromDataTab(true) {}

const char *RegressionController::title() {
  if (displaySeriesNameAsTitle()) {
    return Store::SeriesTitle(m_series);
  }
  return I18n::translate(I18n::Message::RegressionModel);
}

ViewController::TitlesDisplay RegressionController::titlesDisplay() {
  if (displaySeriesNameAsTitle()) {
    return ViewController::TitlesDisplay::DisplayLastTitle;
  }
  return ViewController::TitlesDisplay::DisplayLastTwoTitles;
}

void RegressionController::didBecomeFirstResponder() {
  Model::Type type = m_store->seriesRegressionType(m_series);
  int initialIndex = std::max(0, IndexOfModelType(type));
  if (initialIndex >= numberOfRows()) {
    assert(type == Model::Type::LinearApbx &&
           GlobalPreferences::sharedGlobalPreferences->regressionAppVariant() ==
               CountryPreferences::RegressionApp::Default);
    // Type is hidden for selected country, select the first line.
    initialIndex = 0;
  } else {
    assert(type == Model::Type::None || type == ModelTypeAtIndex(initialIndex));
  }
  selectCell(initialIndex);
  SelectableListViewController<
      MemoizedListViewDataSource>::didBecomeFirstResponder();
}

bool RegressionController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    assert(m_series > -1);
    m_store->setSeriesRegressionType(m_series, ModelTypeAtIndex(selectedRow()));
    StackViewController *stack =
        static_cast<StackViewController *>(parentResponder());
    stack->popUntilDepth(
        Shared::InteractiveCurveViewController::k_graphControllerStackDepth,
        true);
    return true;
  }
  if (event == Ion::Events::Left) {
    StackViewController *stack =
        static_cast<StackViewController *>(parentResponder());
    stack->pop();
    return true;
  }
  return false;
}
KDCoordinate RegressionController::nonMemoizedRowHeight(int j) {
  assert(j >= 0 && j < numberOfRows());
  MenuCell<MessageTextView, ExpressionView> tempCell;
  return heightForCellAtIndexWithWidthInit(&tempCell, j);
}

HighlightCell *RegressionController::reusableCell(int index, int type) {
  assert(index >= 0);
  assert(index < k_numberOfCells);
  return &m_regressionCells[index];
}

void RegressionController::willDisplayCellForIndex(HighlightCell *cell,
                                                   int index) {
  assert(index >= 0 && index < numberOfRows());
  MenuCell<MessageTextView, ExpressionView> *castedCell =
      static_cast<MenuCell<MessageTextView, ExpressionView> *>(cell);
  Model *model = m_store->regressionModel(ModelTypeAtIndex(index));
  castedCell->label()->setMessage(model->name());
  castedCell->subLabel()->setLayout(model->templateLayout());
}

}  // namespace Regression
