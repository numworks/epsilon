#ifndef STATISTICS_CALCULATION_CONTROLLER_H
#define STATISTICS_CALCULATION_CONTROLLER_H

#include <poincare/print_float.h>
#include <poincare/preferences.h>
#include "../store.h"
#include "calculation_selectable_table_view.h"
#include <apps/shared/hideable_even_odd_cell.h>
#include <apps/shared/prefaced_table_view.h>
#include <apps/shared/separator_even_odd_buffer_text_cell.h>
#include <apps/shared/store_title_cell.h>
#include <apps/shared/double_pair_table_controller.h>

namespace Statistics {

class CalculationController : public Shared::DoublePairTableController, public Shared::PrefacedTableView::MarginDelegate {

public:
  CalculationController(Escher::Responder * parentResponder, Escher::ButtonRowController * header, Store * store);

  // TableViewDataSource
  int numberOfRows() const override { return k_fixedNumberOfRows + m_store->totalNumberOfModes(); }
  int numberOfColumns() const override;
  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int i, int j) override;
  KDCoordinate columnWidth(int i) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;

  // ViewController
  Escher::View * view() override { return &m_tableView; }
  TELEMETRY_ID("Calculation");

  // MarginDelegate
  KDCoordinate prefaceMargin(Escher::TableView * preface) override;

private:
  static constexpr int k_fixedNumberOfRows = 17;
  static constexpr int k_numberOfCalculationCells = 3 * k_maxNumberOfDisplayableRows;
  static constexpr int k_numberOfSeriesTitleCells = 3;
  static constexpr int k_numberOfCalculationTitleCells = k_maxNumberOfDisplayableRows;
  static constexpr int k_numberOfHeaderColumns = 2; // Title & Symbol

  static constexpr int k_calculationTitleCellType = 0;
  static constexpr int k_calculationSymbolCellType = 1;
  static constexpr int k_calculationModeTitleCellType = 2;
  static constexpr int k_calculationModeSymbolCellType = 3;
  static constexpr int k_calculationCellType = 4;
  static constexpr int k_seriesTitleCellType = 5;
  static constexpr int k_hideableCellType = 6;
  static constexpr KDCoordinate k_calculationTitleCellWidth = 161;
  /* FIXME: 7 in CalculationSymbolCellWidth and k_calculationCellWidth stands
   * for KDFont::SmallFont->glyphSize().width(). */
  static constexpr KDCoordinate CalculationSymbolCellWidth(int maxChars) { return maxChars * 7 + 2 * Escher::Metric::CellVerticalElementMargin; }
  /* Margins from EvenOddCell::layoutSubviews (and derived classes
   * implementations) must be accounted for here, along with the separator
   * width from SeparatorEvenOddBufferTextCell. */
  static constexpr KDCoordinate k_calculationCellWidth = 7 * Poincare::PrintFloat::glyphLengthForFloatWithPrecision(Poincare::Preferences::VeryLargeNumberOfSignificantDigits) + 2 * Escher::EvenOddCell::k_horizontalMargin + Escher::EvenOddCell::k_separatorWidth;

  Shared::DoublePairStore * store() const override { return m_store; }

  Shared::PrefacedTableView m_tableView;
  Shared::StoreTitleCell m_seriesTitleCells[k_numberOfSeriesTitleCells];
  Escher::EvenOddMessageTextCell m_calculationTitleCells[k_numberOfCalculationTitleCells];
  Escher::EvenOddMessageTextCell m_calculationSymbolCells[k_numberOfCalculationTitleCells];
  Escher::EvenOddBufferTextCell m_calculationModeTitleCells[k_numberOfCalculationTitleCells];
  Escher::EvenOddBufferTextCell m_calculationModeSymbolCells[k_numberOfCalculationTitleCells];
  Shared::SeparatorEvenOddBufferTextCell m_calculationCells[k_numberOfCalculationCells];
  Shared::HideableEvenOddCell m_hideableCell[k_numberOfHeaderColumns];
  Store * m_store;
};

}


#endif
