#include "results_between_within_table.h"

#include <omg/unreachable.h>

#include "inference/controllers/anova/results_tab_controller.h"
#include "inference/models/anova_test.h"

using namespace Escher;

namespace Inference::ANOVA {

ResultsBetweenWithinTableCell::ResultsBetweenWithinTableCell(
    Escher::Responder* parentResponder, ANOVATest* test,
    BetweenWithinController* betweenWithinTableController,
    Escher::ScrollViewDelegate* scrollViewDelegate)
    : CategoricalTableCell(parentResponder, this, scrollViewDelegate),
      m_inference(test),
      m_betweenWithinTableController(betweenWithinTableController) {
  m_selectableTableView.margins()->setBottom(Metric::CellSeparatorThickness);
}

void ResultsBetweenWithinTableCell::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    if (selectedRow() < 0) {
      selectColumn(1);
    }
    // The number of data might have changed
    if (selectedRow() >= numberOfRows() ||
        selectedColumn() >= numberOfColumns()) {
      selectRow(1);
      selectColumn(1);
    }
  }
  CategoricalTableCell::handleResponderChainEvent(event);
}

void ResultsBetweenWithinTableCell::drawRect(KDContext* ctx,
                                             KDRect rect) const {
  CategoricalTableCell::drawRect(ctx, rect);
  // Draw over the next cell border to hide it
  ctx->fillRect(KDRect(0, bounds().height() - Metric::CellSeparatorThickness,
                       bounds().width(), Metric::CellSeparatorThickness),
                m_selectableTableView.backgroundColor());
}

// The row index refers to the "inner" table
constexpr static const char* HeaderAtRow(int row) {
  assert(row >= 0 && row <= ResultsBetweenWithinTableCell::k_numberOfInnerRows);
  switch (row) {
    case 0:
      return I18n::translate(I18n::Message::SquareSum);
    case 1:
      return I18n::translate(I18n::Message::DegreesOfFreedom);
    case 2:
      return I18n::translate(I18n::Message::MeanSquares);
    default:
      OMG::unreachable();
  }
}

// The row index refers to the "inner" table
constexpr static const char* SymbolAtRow(int row) {
  assert(row >= 0 && row <= ResultsBetweenWithinTableCell::k_numberOfInnerRows);
  switch (row) {
    case 0:
      return I18n::translate(I18n::Message::SquareSumSymbol);
    case 1:
      return I18n::translate(I18n::Message::DegreesOfFreedomSymbol);
    case 2:
      return I18n::translate(I18n::Message::MeanSquaresSymbol);
    default:
      OMG::unreachable();
  }
}

void ResultsBetweenWithinTableCell::fillHeaderCellForLocation(
    Escher::HighlightCell* cell, int column, int row) {
  headerCellType* myCell = static_cast<headerCellType*>(cell);

  if (row < numberOfHeaderRows()) {
    int innerColumnIndex = column - numberOfHeaderColumns();
    assert(innerColumnIndex <= k_numberOfInnerColumns);
    // Column title
    switch (innerColumnIndex) {
      case 0:
        myCell->setText(I18n::translate(I18n::Message::Between));
        break;
      case 1:
        myCell->setText(I18n::translate(I18n::Message::Within));
        break;
      default:
        OMG::unreachable();
    }
    myCell->setAlignment(KDGlyph::k_alignCenter, KDGlyph::k_alignCenter);
  } else {
    assert(column < numberOfHeaderColumns());
    if (column == 0) {
      // Row title
      myCell->setText(HeaderAtRow(row - numberOfHeaderRows()));
      myCell->setAlignment(KDGlyph::k_alignRight, KDGlyph::k_alignCenter);
      myCell->setTextColor(KDColorBlack);
    } else {
      assert(column == 1);
      // Row symbol
      myCell->setText(SymbolAtRow(row - numberOfHeaderRows()));
      myCell->setAlignment(KDGlyph::k_alignCenter, KDGlyph::k_alignCenter);
      myCell->setTextColor(Palette::GrayDark);
    }
  }
}

void ResultsBetweenWithinTableCell::fillInnerCellForLocation(
    Escher::HighlightCell* cell, int column, int row) {
  assert(row >= 0 && row < innerNumberOfRows());
  assert(column >= 0 && column < innerNumberOfColumns());
  innerCellType* myCell = static_cast<innerCellType*>(cell);
  const ANOVATest::CalculatedValues& values =
      (column == 0) ? m_inference->results().between
                    : m_inference->results().within;
  switch (row) {
    case 0:
      PrintValueInTextHolder(values.sumOfSquares, myCell);
      break;
    case 1:
      PrintValueInTextHolder(values.degreesOfFreedom, myCell);
      break;
    case 2:
      PrintValueInTextHolder(values.meanSquares, myCell);
      break;
    default:
      OMG::unreachable();
  }
}

CategoricalController* ResultsBetweenWithinTableCell::categoricalController() {
  return m_betweenWithinTableController;
}

}  // namespace Inference::ANOVA
