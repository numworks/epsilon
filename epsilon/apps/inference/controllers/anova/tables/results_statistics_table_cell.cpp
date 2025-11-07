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

void ResultsStatisticsTableCell::fillInnerCellForLocation(
    Escher::HighlightCell* cell, int column, int row) {
  assert(row >= 0 && row < innerNumberOfRows());
  assert(column >= 0 && column < innerNumberOfColumns());

  InferenceEvenOddBufferCell* myCell =
      static_cast<InferenceEvenOddBufferCell*>(cell);

  const ANOVATest::GroupData& groupStatistics =
      m_inference->groupStatistics(column);

  switch (row) {
    case 0:
      PrintValueInTextHolder(groupStatistics.nSamples, myCell);
      break;
    case 1:
      PrintValueInTextHolder(groupStatistics.mean, myCell);
      break;
    case 2:
      PrintValueInTextHolder(groupStatistics.sampleStdDeviation, myCell);
      break;
    case 3:
      PrintValueInTextHolder(groupStatistics.sampleVariance, myCell);
      break;
    default:
      OMG::unreachable();
  }

  myCell->setEven(row % 2 == 1);
}

void ResultsStatisticsTableCell::fillCellForLocation(
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
      // TODO: simplify and factorize with InputANOVATableCell
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
      myCell->setEven(true);
    } else {
      // Row title
      assert(row == 1 || row == 2 || row == 3 || row == 4);
      switch (row) {
        case 1:
          myCell->setText(I18n::translate(I18n::Message::SampleSize));
          break;
        case 2:
          myCell->setText(I18n::translate(I18n::Message::SampleMean));
          break;
        case 3:
          myCell->setText(I18n::translate(I18n::Message::SampleSTD));
          break;
        case 4:
          myCell->setText(I18n::translate(I18n::Message::SampleVariance));
          break;
        default:
          OMG::unreachable();
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

CategoricalController* ResultsStatisticsTableCell::categoricalController() {
  return m_statisticsResultsController;
}

}  // namespace Inference
