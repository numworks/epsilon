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
      OMG_UNREACHABLE;
  }
}

// The row index refers to the "inner" table
constexpr static const char* HeaderAtRow(int row) {
  assert(row >= 0 && row < ResultsStatisticsTable::k_numberOfInnerRows);
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
      OMG_UNREACHABLE;
  }
}

// The row index refers to the "inner" table
constexpr static const char* SymbolAtRow(int row) {
  assert(row >= 0 && row < ResultsStatisticsTable::k_numberOfInnerRows);
  switch (row) {
    case 0:
      return I18n::translate(I18n::Message::N);
    case 1:
      return I18n::translate(I18n::Message::MeanSymbol);
    case 2:
      return I18n::translate(I18n::Message::S);
    case 3:
      return I18n::translate(I18n::Message::SampleVarianceSymbol);
    default:
      OMG_UNREACHABLE;
  }
}

void ResultsStatisticsTable::fillColumnTitleForLocation(
    Escher::HighlightCell* cell, int innerColumn) {
  assert(innerColumn >= 0 && innerColumn < innerNumberOfColumns());
  headerCellType* myCell = static_cast<headerCellType*>(cell);
  OMG::String<k_groupTitleBufferSize> groupTitle = GroupTitle(innerColumn);
  myCell->setText(groupTitle.data());
  myCell->setAlignment(KDGlyph::k_alignCenter, KDGlyph::k_alignCenter);
  myCell->setTextColor(KDColorBlack);
}
void ResultsStatisticsTable::fillRowTitleForLocation(
    Escher::HighlightCell* cell, int innerRow) {
  assert(innerRow >= 0 && innerRow < innerNumberOfRows());
  headerCellType* myCell = static_cast<headerCellType*>(cell);
  myCell->setText(HeaderAtRow(innerRow));
  myCell->setAlignment(KDGlyph::k_alignRight, KDGlyph::k_alignCenter);
  myCell->setTextColor(KDColorBlack);
}

void ResultsStatisticsTable::fillRowSymbolForLocation(
    Escher::HighlightCell* cell, int innerRow) {
  assert(innerRow >= 0 && innerRow < innerNumberOfRows());
  headerCellType* myCell = static_cast<headerCellType*>(cell);
  myCell->setText(SymbolAtRow(innerRow));
  myCell->setAlignment(KDGlyph::k_alignCenter, KDGlyph::k_alignCenter);
  myCell->setTextColor(Palette::GrayDark);
}

void ResultsStatisticsTable::fillInnerCellForLocation(
    Escher::HighlightCell* cell, int innerColumn, int innerRow) {
  assert(innerRow >= 0 && innerRow < innerNumberOfRows());
  assert(innerColumn >= 0 && innerColumn < innerNumberOfColumns());
  innerCellType* myCell = static_cast<innerCellType*>(cell);
  const ANOVATest::GroupData& groupStatistics =
      m_inference->groupStatistics(innerColumn);
  PrintValueInTextHolder(GroupStatisticsResultAtRow(groupStatistics, innerRow),
                         myCell);
}

CategoricalController* ResultsStatisticsTable::categoricalController() {
  return m_statisticsResultsController;
}

}  // namespace Inference::ANOVA
