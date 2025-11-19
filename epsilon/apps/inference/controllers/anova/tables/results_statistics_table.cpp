#include "results_statistics_table.h"

#include <omg/unreachable.h>

#include "inference/controllers/anova/results_tab_controller.h"
#include "inference/controllers/tables/header_titles.h"
#include "inference/models/anova_test.h"

using namespace Escher;

namespace Inference::ANOVA {

ResultsStatisticsTable::ResultsStatisticsTable(
    Escher::Responder* parentResponder, ANOVATest* test,
    StatisticsController* statisticsTableController,
    Escher::ScrollViewDelegate* scrollViewDelegate)
    : CategoricalTableCell(parentResponder, this, scrollViewDelegate),
      m_inference(test),
      m_statisticsResultsController(statisticsTableController) {
  m_selectableTableView.margins()->setBottom(Metric::CellSeparatorThickness);
}

void ResultsStatisticsTable::handleResponderChainEvent(
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

void ResultsStatisticsTable::drawRect(KDContext* ctx, KDRect rect) const {
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
      return I18n::translate(I18n::Message::SampleSizeShort);
    case 1:
      return I18n::translate(I18n::Message::SampleMeanShort);
    case 2:
      return I18n::translate(I18n::Message::SampleSTDShort);
    case 3:
      return I18n::translate(I18n::Message::SampleVariance);
    default:
      OMG::unreachable();
  }
}

void ResultsStatisticsTable::fillInnerCellForLocation(
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

void ResultsStatisticsTable::fillCellForLocation(Escher::HighlightCell* cell,
                                                 int column, int row) {
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
      OMG::String<k_groupTitleBufferSize> groupTitle = GroupTitle(column - 1);
      myCell->setText(groupTitle.data());
    } else {
      // Row title
      myCell->setText(HeaderAtRow(row - 1));
    }
    myCell->setAlignment(KDGlyph::k_alignRight, KDGlyph::k_alignCenter);
    myCell->setTextColor(KDColorBlack);
  } else {
    assert(type == k_typeOfInnerCells);
    fillInnerCellForLocation(cell, column - 1, row - 1);
  }
  myCell->setEven(row % 2 == 0);
}

CategoricalController* ResultsStatisticsTable::categoricalController() {
  return m_statisticsResultsController;
}

}  // namespace Inference::ANOVA
