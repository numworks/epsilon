#include "graph_options_controller.h"

#include <apps/shared/poincare_helpers.h>
#include <assert.h>
#include <escher/clipboard.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>

#include "../app.h"
#include "graph_controller.h"
#include "regression_controller.h"

using namespace Shared;
using namespace Escher;
using namespace Poincare;

namespace Regression {

GraphOptionsController::GraphOptionsController(Responder *parentResponder,
                                               InteractiveCurveViewRange *range,
                                               Store *store,
                                               CurveViewCursor *cursor,
                                               GraphController *graphController)
    : ExplicitSelectableListViewController(parentResponder),
      m_removeRegressionCell(
          &(this->m_selectableListView), I18n::Message::RemoveRegression,
          Invocation::Builder<GraphOptionsController>(
              [](GraphOptionsController *controller, void *sender) {
                controller->removeRegression();
                return true;
              },
              this),
          ButtonCell::Style::EmbossedLight),
      m_goToParameterController(this, range, store, cursor, graphController),
      m_residualPlotCellController(parentResponder, store),
      m_store(store),
      m_graphController(graphController) {
  m_residualPlotCell.label()->setMessage(I18n::Message::ResidualPlot);
  m_rCell.label()->setLayout("r"_l);
  m_changeRegressionCell.label()->setMessage(I18n::Message::RegressionModel);
  m_xParameterCell.label()->setMessage(I18n::Message::XPrediction);
  m_yParameterCell.label()->setMessage(I18n::Message::YPrediction);
  m_regressionEquationCell.label()->setParentResponder(&m_selectableListView);
  m_regressionEquationCell.subLabel()->setMessage(
      I18n::Message::RegressionEquation);
  // Hide cells by default to prevent any forbidden layouting
  m_regressionEquationCell.setVisible(false);
  m_rCell.setVisible(false);
  m_r2Cell.setVisible(false);
  m_residualPlotCell.setVisible(false);
}

void GraphOptionsController::removeRegression() {
  int series = m_graphController->selectedSeriesIndex();
  m_store->setSeriesRegressionType(series, Model::Type::None);
  static_cast<StackViewController *>(parentResponder())->pop();
}

const char *GraphOptionsController::title() {
  return Store::SeriesTitle(m_graphController->selectedSeriesIndex());
}

void GraphOptionsController::viewWillAppear() {
  m_regressionEquationCell.setVisible(displayRegressionEquationCell());
  m_rCell.setVisible(displayRCell());
  m_r2Cell.setVisible(displayR2Cell());
  m_residualPlotCell.setVisible(displayResidualPlotCell());
  // m_regressionEquationCell may have changed size
  m_regressionEquationCell.label()->resetScroll();

  int series = m_graphController->selectedSeriesIndex();
  Regression::Model *model = m_store->modelForSeries(series);

  // Change regression cell
  m_changeRegressionCell.subLabel()->setMessage(model->name());

  const int significantDigits = Preferences::VeryLargeNumberOfSignificantDigits;
  Preferences::PrintFloatMode displayMode =
      Preferences::PrintFloatMode::Decimal;

  // Regression equation cell
  double *coefficients = m_store->coefficientsForSeries(
      series, m_graphController->globalContext());
  m_regressionEquationCell.label()->setLayout(
      model->equationLayout(coefficients, "y", significantDigits, displayMode));

  // r and r2 cells
  RCell *rCells[2] = {&m_rCell, &m_r2Cell};
  for (int i = 0; i < 2; i++) {
    if (!rCells[i]->isVisible()) {
      continue;
    }
    bool isRCell = rCells[i] == &m_rCell;
    if (!isRCell) {
      Model::Type type = m_store->seriesRegressionType(
          m_graphController->selectedSeriesIndex());
      assert(Store::DisplayR2(type) != Store::DisplayRSquared(type));
      m_r2Cell.label()->setLayout(
          Store::DisplayR2(type) ? "R2"_l : "r"_l ^ KSuperscriptL("2"_l));
    }
    if (Preferences::SharedPreferences()->examMode().forbidStatsDiagnostics()) {
      rCells[i]->label()->setTextColor(Palette::GrayDark);
      rCells[i]->accessory()->setTextColor(Palette::GrayDark);
      rCells[i]->accessory()->setText(I18n::translate(I18n::Message::Disabled));
      continue;
    }
    constexpr int bufferSize = PrintFloat::charSizeForFloatsWithPrecision(
        Preferences::VeryLargeNumberOfSignificantDigits);
    char buffer[bufferSize];
    double value = isRCell ? m_store->correlationCoefficient(series)
                           : m_store->determinationCoefficientForSeries(
                                 series, m_graphController->globalContext());
    int insertedChars =
        Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(
            value, buffer, bufferSize, significantDigits, displayMode);
    assert(insertedChars < bufferSize);
    (void)insertedChars;  // Silence warnings
    rCells[i]->accessory()->setText(buffer);
  }

  m_selectableListView.reloadData();
}

bool GraphOptionsController::handleEvent(Ion::Events::Event event) {
  StackViewController *stack =
      static_cast<StackViewController *>(parentResponder());
  if (event == Ion::Events::Left &&
      stack->depth() >
          Shared::InteractiveCurveViewController::k_graphControllerStackDepth +
              1) {
    /* We only allow popping with Left if there is another menu beneath this
     * one. */
    stack->pop();
    return true;
  }

  HighlightCell *cell = selectedCell();
  if ((event == Ion::Events::Copy || event == Ion::Events::Cut) ||
      event == Ion::Events::Sto || event == Ion::Events::Var) {
    if (cell == &m_regressionEquationCell) {
      Layout l = m_regressionEquationCell.label()->layout();
      if (!l.isUninitialized()) {
        constexpr int bufferSize = TextField::MaxBufferSize();
        char buffer[bufferSize];
        l.serializeParsedExpression(buffer, bufferSize, nullptr);
        if (event == Ion::Events::Sto || event == Ion::Events::Var) {
          App::app()->storeValue(buffer);
        } else {
          Escher::Clipboard::SharedClipboard()->store(buffer);
        }
        return true;
      }
    }
    return Escher::ExplicitSelectableListViewController::handleEvent(event);
  }

  if (cell == &m_changeRegressionCell &&
      m_changeRegressionCell.canBeActivatedByEvent(event)) {
    RegressionController *controller = App::app()->regressionController();
    controller->setSeries(m_graphController->selectedSeriesIndex());
    controller->setDisplayedFromDataTab(false);
    stack->push(controller);
    return true;
  } else if (cell == &m_residualPlotCell &&
             m_residualPlotCell.canBeActivatedByEvent(event)) {
    m_residualPlotCellController.setSeries(
        m_graphController->selectedSeriesIndex());
    stack->push(&m_residualPlotCellController);
    return true;
  } else if ((cell == &m_xParameterCell || cell == &m_yParameterCell) &&
             static_cast<AbstractMenuCell *>(cell)->canBeActivatedByEvent(
                 event)) {
    m_goToParameterController.setXPrediction(cell == &m_xParameterCell);
    stack->push(&m_goToParameterController);
    return true;
  }

  return false;
}

HighlightCell *GraphOptionsController::cell(int row) {
  assert(row >= 0 && row < k_maxNumberOfRows);
  if (GlobalPreferences::SharedGlobalPreferences()->regressionAppVariant() ==
      CountryPreferences::RegressionApp::Default) {
    HighlightCell *cells[k_maxNumberOfRows] = {&m_changeRegressionCell,
                                               &m_regressionEquationCell,
                                               &m_rCell,
                                               &m_r2Cell,
                                               &m_xParameterCell,
                                               &m_yParameterCell,
                                               &m_residualPlotCell,
                                               &m_removeRegressionCell};
    return cells[row];
  }
  assert(GlobalPreferences::SharedGlobalPreferences()->regressionAppVariant() ==
         CountryPreferences::RegressionApp::Variant1);
  HighlightCell *cells[k_maxNumberOfRows] = {&m_changeRegressionCell,
                                             &m_regressionEquationCell,
                                             &m_rCell,
                                             &m_r2Cell,
                                             &m_residualPlotCell,
                                             &m_xParameterCell,
                                             &m_yParameterCell,
                                             &m_removeRegressionCell};
  return cells[row];
}
bool GraphOptionsController::displayRegressionEquationCell() const {
  return m_store->coefficientsAreDefined(
      m_graphController->selectedSeriesIndex(),
      m_graphController->globalContext());
}

bool GraphOptionsController::displayRCell() const {
  return displayRegressionEquationCell() &&
         m_store->seriesSatisfies(m_graphController->selectedSeriesIndex(),
                                  Store::DisplayR);
}

bool GraphOptionsController::displayR2Cell() const {
  Model::Type type =
      m_store->seriesRegressionType(m_graphController->selectedSeriesIndex());
  return displayRegressionEquationCell() &&
         (Store::DisplayR2(type) || Store::DisplayRSquared(type));
}

bool GraphOptionsController::displayResidualPlotCell() const {
  return m_store->coefficientsAreDefined(
      m_graphController->selectedSeriesIndex(),
      m_graphController->globalContext(), true);
}

}  // namespace Regression
