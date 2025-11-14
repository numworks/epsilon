#include "results_between_within_table_cell.h"

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

void ResultsBetweenWithinTableCell::fillInnerCellForLocation(
    Escher::HighlightCell* cell, int column, int row) {
  assert(row >= 0 && row < innerNumberOfRows());
  assert(column >= 0 && column < innerNumberOfColumns());

  InferenceEvenOddBufferCell* myCell =
      static_cast<InferenceEvenOddBufferCell*>(cell);

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

  myCell->setEven(row % 2 == 1);
}

void ResultsBetweenWithinTableCell::fillCellForLocation(
    Escher::HighlightCell* cell, int column, int row) {
  assert(row >= 0 && row < numberOfRows());
  assert(column >= 0 && column < numberOfColumns());

  int type = typeAtLocation(column, row);
  if (type == k_typeOfTopLeftCell) {
    return;
  }
  if (type == k_typeOfHeaderCells) {
    InferenceEvenOddBufferCell* myCell =
        static_cast<InferenceEvenOddBufferCell*>(cell);
    if (row == 0) {
      // Column title
      assert(column == 1 || column == 2);
      if (column == 1) {
        myCell->setText(I18n::translate(I18n::Message::Between));
      } else {
        myCell->setText(I18n::translate(I18n::Message::Within));
      }
      myCell->setAlignment(KDGlyph::k_alignCenter, KDGlyph::k_alignCenter);
      myCell->setEven(true);
    } else {
      // Row title
      assert(row == 1 || row == 2 || row == 3);
      if (row == 1) {
        myCell->setText(I18n::translate(I18n::Message::SquareSum));
      } else if (row == 2) {
        myCell->setText(I18n::translate(I18n::Message::DegreesOfFreedom));
      } else {
        myCell->setText(I18n::translate(I18n::Message::MeanSquares));
      }
      myCell->setAlignment(KDGlyph::k_alignRight, KDGlyph::k_alignCenter);
      myCell->setEven(static_cast<bool>((row + 1) % 2));
    }
    myCell->setTextColor(KDColorBlack);
  } else {
    assert(type == k_typeOfInnerCells);
    fillInnerCellForLocation(cell, column - 1, row - 1);
  }
}

CategoricalController* ResultsBetweenWithinTableCell::categoricalController() {
  return m_betweenWithinTableController;
}

}  // namespace Inference::ANOVA
