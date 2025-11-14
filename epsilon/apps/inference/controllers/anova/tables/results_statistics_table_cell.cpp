#include "results_statistics_table_cell.h"

#include <omg/unreachable.h>
#include <poincare/print.h>

#include "inference/controllers/anova/results_anova_controller.h"
#include "inference/models/anova_test.h"

using namespace Escher;

namespace Inference {

ResultsStatisticsTableCell::ResultsStatisticsTableCell(
    Escher::Responder* parentResponder, ANOVATest* test,
    StatisticsController* statisticsTableController,
    Escher::ScrollViewDelegate* scrollViewDelegate)
    : CategoricalTableCell(parentResponder, this, scrollViewDelegate),
      m_inference(test),
      m_statisticsResultsController(statisticsTableController) {
  m_selectableTableView.margins()->setBottom(Metric::CellSeparatorThickness);
}

void ResultsStatisticsTableCell::handleResponderChainEvent(
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

void ResultsStatisticsTableCell::drawRect(KDContext* ctx, KDRect rect) const {
  CategoricalTableCell::drawRect(ctx, rect);
  // Draw over the next cell border to hide it
  ctx->fillRect(KDRect(0, bounds().height() - Metric::CellSeparatorThickness,
                       bounds().width(), Metric::CellSeparatorThickness),
                m_selectableTableView.backgroundColor());
}

constexpr static double GroupStatisticsResultAtRow(
    const ANOVATest::GroupData& groupStatistics, int row) {
  assert(row >= 0 && row <= 3);
  switch (row) {
    case 0:
      return groupStatistics.nSamples;
    case 1:
      return groupStatistics.mean;
    case 2:
      return groupStatistics.sampleStdDeviation;
    case 3:
      return groupStatistics.sampleVariance;
    default:
      OMG::unreachable();
  }
}

// The row index refers to the "inner" table
constexpr static const char* HeaderAtRow(int row) {
  assert(row >= 0 && row <= 3);
  switch (row) {
    case 0:
      return I18n::translate(I18n::Message::SampleSize);
    case 1:
      return I18n::translate(I18n::Message::SampleMean);
    case 2:
      return I18n::translate(I18n::Message::SampleSTDShort);
    case 3:
      return I18n::translate(I18n::Message::SampleVariance);
    default:
      OMG::unreachable();
  }
}

void ResultsStatisticsTableCell::fillInnerCellForLocation(
    Escher::HighlightCell* cell, int column, int row) {
  assert(row >= 0 && row < innerNumberOfRows());
  assert(column >= 0 && column < innerNumberOfColumns());

  InferenceEvenOddBufferCell* myCell =
      static_cast<InferenceEvenOddBufferCell*>(cell);

  const ANOVATest::GroupData& groupStatistics =
      m_inference->groupStatistics(column);

  PrintValueInTextHolder(GroupStatisticsResultAtRow(groupStatistics, row),
                         myCell);
}

void ResultsStatisticsTableCell::fillCellForLocation(
    Escher::HighlightCell* cell, int column, int row) {
  assert(row >= 0 && row < numberOfRows());
  assert(column >= 0 && column < numberOfColumns());

  int type = typeAtLocation(column, row);
  if (type == k_typeOfTopLeftCell) {
    return;
  }
  InferenceEvenOddBufferCell* myCell =
      static_cast<InferenceEvenOddBufferCell*>(cell);
  if (type == k_typeOfHeaderCells) {
    if (row == 0) {
      // Column title
      // TODO: simplify and factorize with InputDataANOVATableCell
      char digit;
      int groupIndex = column - 1;
      assert(groupIndex <= '9' - '1');
      digit = '1' + groupIndex;
      constexpr int bufferSize = 20;
      char txt[bufferSize];
      Poincare::Print::CustomPrintf(txt, bufferSize, "%s %c",
                                    I18n::translate(I18n::Message::Group),
                                    digit);
      myCell->setText(txt);
      myCell->setAlignment(KDGlyph::k_alignCenter, KDGlyph::k_alignCenter);
    } else {
      // Row title
      myCell->setText(HeaderAtRow(row - 1));
      myCell->setAlignment(KDGlyph::k_alignRight, KDGlyph::k_alignCenter);
    }
    myCell->setTextColor(KDColorBlack);
  } else {
    assert(type == k_typeOfInnerCells);
    fillInnerCellForLocation(cell, column - 1, row - 1);
  }
  myCell->setEven(row % 2 == 0);
}

CategoricalController* ResultsStatisticsTableCell::categoricalController() {
  return m_statisticsResultsController;
}

}  // namespace Inference
