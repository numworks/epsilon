#include "graph_options_controller.h"

#include <apps/shared/poincare_helpers.h>
#include <assert.h>
#include <escher/clipboard.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/vertical_offset_layout.h>

#include "../app.h"
#include "graph_controller.h"
#include "regression_controller.h"

using namespace Shared;
using namespace Escher;
using namespace Poincare;

namespace Regression {

GraphOptionsController::GraphOptionsController(
    Responder *parentResponder,
    Escher::InputEventHandlerDelegate *inputEventHandlerDelegate,
    InteractiveCurveViewRange *range, Store *store, CurveViewCursor *cursor,
    GraphController *graphController)
    : ExplicitSelectableListViewController(parentResponder),
      m_changeRegressionCell(I18n::Message::RegressionModel),
      m_regressionEquationCell(&m_selectableListView,
                               I18n::Message::RegressionEquation),
      m_rCell(&m_selectableListView, I18n::Message::Default,
              KDFont::Size::Large),
      m_r2Cell(&m_selectableListView, I18n::Message::Default,
               KDFont::Size::Large),
      m_residualPlotCell(I18n::Message::ResidualPlot),
      m_xParameterCell(I18n::Message::XPrediction),
      m_yParameterCell(I18n::Message::YPrediction),
      m_removeRegressionCell(
          &(this->m_selectableListView), I18n::Message::RemoveRegression,
          Invocation::Builder<GraphOptionsController>(
              [](GraphOptionsController *controller, void *sender) {
                controller->removeRegression();
                return true;
              },
              this)),
      m_goToParameterController(this, inputEventHandlerDelegate, range, store,
                                cursor, graphController),
      m_residualPlotCellController(parentResponder, store),
      m_store(store),
      m_graphController(graphController) {
  m_rCell.setLayout(CodePointLayout::Builder('r'));
  m_r2Cell.setLayout(HorizontalLayout::Builder(
      {CodePointLayout::Builder('R'),
       VerticalOffsetLayout::Builder(
           CodePointLayout::Builder('2'),
           VerticalOffsetLayoutNode::VerticalPosition::Superscript)}));
}

void GraphOptionsController::removeRegression() {
  int series = m_graphController->selectedSeriesIndex();
  m_store->setSeriesRegressionType(series, Model::Type::None);
  static_cast<StackViewController *>(parentResponder())->pop();
}

const char *GraphOptionsController::title() {
  return Store::SeriesTitle(m_graphController->selectedSeriesIndex());
}

void GraphOptionsController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  Container::activeApp()->setFirstResponder(&m_selectableListView);
}

void GraphOptionsController::viewWillAppear() {
  m_regressionEquationCell.setVisible(displayRegressionEquationCell());
  m_rCell.setVisible(displayRCell());
  m_r2Cell.setVisible(displayR2Cell());
  m_residualPlotCell.setVisible(displayResidualPlotCell());
  // m_regressionEquationCell may have changed size
  m_regressionEquationCell.reloadScroll();
  resetMemoization();
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
  if (event == Ion::Events::OK || event == Ion::Events::EXE ||
      event == Ion::Events::Right) {
    if (cell == &m_changeRegressionCell) {
      RegressionController *controller = App::app()->regressionController();
      controller->setSeries(m_graphController->selectedSeriesIndex());
      controller->setDisplayedFromDataTab(false);
      stack->push(controller);
      return true;
    } else if (cell == &m_residualPlotCell) {
      m_residualPlotCellController.setSeries(
          m_graphController->selectedSeriesIndex());
      stack->push(&m_residualPlotCellController);
      return true;
    } else if (cell == &m_xParameterCell || cell == &m_yParameterCell) {
      m_goToParameterController.setXPrediction(cell == &m_xParameterCell);
      stack->push(&m_goToParameterController);
      return true;
    }
  } else if ((event == Ion::Events::Copy || event == Ion::Events::Cut) ||
             event == Ion::Events::Sto || event == Ion::Events::Var) {
    if (cell == &m_r2Cell) {
      if (event == Ion::Events::Sto || event == Ion::Events::Var) {
        App::app()->storeValue(m_r2Cell.text());
      } else {
        Escher::Clipboard::SharedClipboard()->store(m_r2Cell.text());
      }
      return true;
    } else if (cell == &m_regressionEquationCell) {
      Layout l = m_regressionEquationCell.layout();
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
  }
  return false;
}

HighlightCell *GraphOptionsController::cell(int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  HighlightCell *cells[k_maxNumberOfRows] = {
      &m_changeRegressionCell,
      &m_regressionEquationCell,
      &m_r2Cell,
      &m_rCell,
      &m_residualPlotCell,
      &m_spacerCell1,
      &m_xParameterCell,
      &m_yParameterCell,
      &m_spacerCell2,
      &m_removeRegressionCell,
  };
  return cells[index];
}

void GraphOptionsController::fillCell(HighlightCell *cell) {
  int series = m_graphController->selectedSeriesIndex();
  Regression::Model *model = m_store->modelForSeries(series);
  if (cell == &m_changeRegressionCell) {
    m_changeRegressionCell.setSubtitle(model->name());
    return;
  }
  const int significantDigits =
      Preferences::sharedPreferences->numberOfSignificantDigits();
  Preferences::PrintFloatMode displayMode =
      Preferences::sharedPreferences->displayMode();
  if (cell == &m_regressionEquationCell) {
    double *coefficients = m_store->coefficientsForSeries(
        series, m_graphController->globalContext());
    m_regressionEquationCell.setLayout(model->equationLayout(
        coefficients, "y", significantDigits, displayMode));
  } else if (cell == &m_rCell || cell == &m_r2Cell) {
    ExpressionTableCellWithMessageWithBuffer *rCell =
        static_cast<ExpressionTableCellWithMessageWithBuffer *>(cell);
    if (Preferences::sharedPreferences->examMode().forbidStatsDiagnostics()) {
      rCell->setTextColor(Palette::GrayDark);
      rCell->setSubLabelMessage(I18n::Message::Disabled);
      rCell->setAccessoryText("");
      return;
    }
    constexpr int bufferSize = PrintFloat::charSizeForFloatsWithPrecision(
        Preferences::VeryLargeNumberOfSignificantDigits);
    char buffer[bufferSize];
    double value = cell == &m_rCell
                       ? m_store->correlationCoefficient(series)
                       : m_store->determinationCoefficientForSeries(
                             series, m_graphController->globalContext());
    Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(
        value, buffer, bufferSize, significantDigits, displayMode);
    rCell->setAccessoryText(buffer);
  }
}

bool GraphOptionsController::displayRegressionEquationCell() const {
  return m_store->coefficientsAreDefined(
      m_graphController->selectedSeriesIndex(),
      m_graphController->globalContext());
}

bool GraphOptionsController::displayRCell() const {
  Model::Type type =
      m_store->seriesRegressionType(m_graphController->selectedSeriesIndex());
  return (type == Regression::Model::Type::LinearApbx ||
          type == Regression::Model::Type::LinearAxpb) &&
         displayRegressionEquationCell();
}

bool GraphOptionsController::displayR2Cell() const {
  return m_store->seriesRegressionType(
             m_graphController->selectedSeriesIndex()) !=
             Regression::Model::Type::Median &&
         displayRegressionEquationCell();
}

bool GraphOptionsController::displayResidualPlotCell() const {
  return displayRegressionEquationCell();
}

}  // namespace Regression
