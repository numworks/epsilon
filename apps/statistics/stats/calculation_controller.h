#ifndef STATISTICS_CALCULATION_CONTROLLER_H
#define STATISTICS_CALCULATION_CONTROLLER_H

#include <escher/alternate_empty_view_controller.h>
#include <escher/button_row_controller.h>
#include <poincare/print_float.h>
#include <poincare/preferences.h>
#include "../store.h"
#include "calculation_selectable_table_view.h"
#include <apps/shared/hideable_even_odd_cell.h>
#include <apps/shared/prefaced_table_view.h>
#include <apps/shared/separator_even_odd_buffer_text_cell.h>
#include <apps/shared/store_title_cell.h>
#include <apps/shared/tab_table_controller.h>

namespace Statistics {

class CalculationController : public Shared::TabTableController, public Escher::ButtonRowDelegate, public Escher::TableViewDataSource, public Escher::AlternateEmptyViewDefaultDelegate, public Shared::PrefacedTableView::MarginDelegate {

public:
  CalculationController(Escher::Responder * parentResponder, Escher::ButtonRowController * header, Store * store);

  // AlternateEmptyViewDefaultDelegate
  bool isEmpty() const override;
  I18n::Message emptyMessage() override;
  Escher::Responder * defaultController() override;

  // TableViewDataSource
  int numberOfRows() const override { return k_totalNumberOfRows; }
  int numberOfColumns() const override;
  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int i, int j) override;
  KDCoordinate columnWidth(int i) override;
  KDCoordinate rowHeight(int j) override { return k_cellHeight; }
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;

  // ViewController
  const char * title() override;
  Escher::View * view() override { return &m_tableView; }
  TELEMETRY_ID("Calculation");

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  // MarginDelegate
  KDCoordinate prefaceMargin(Escher::TableView * preface) override;

private:
  static constexpr int k_totalNumberOfRows = 15;
  static constexpr int k_maxNumberOfDisplayableRows = 11;
  static constexpr int k_numberOfCalculationCells = 3 * k_maxNumberOfDisplayableRows;
  static constexpr int k_numberOfSeriesTitleCells = 3;
  static constexpr int k_numberOfCalculationTitleCells = k_maxNumberOfDisplayableRows;
  static constexpr int k_numberOfHeaderColumns = 2; // Title & Symbol

  static constexpr int k_calculationTitleCellType = 0;
  static constexpr int k_calculationSymbolCellType = 1;
  static constexpr int k_calculationCellType = 2;
  static constexpr int k_seriesTitleCellType = 3;
  static constexpr int k_hideableCellType = 4;
  static constexpr KDCoordinate k_cellHeight = 20;
  static constexpr KDCoordinate k_calculationTitleCellWidth = 161;
  /* FIXME: 7 in k_calculationSymbolCellWidth and k_calculationCellWidth stands
   * for KDFont::SmallFont->glyphSize().width(). */
  static constexpr KDCoordinate k_calculationSymbolCellWidth = 3 * 7 + 2 * Escher::Metric::CellVerticalElementMargin;
  /* Margins from EvenOddCell::layoutSubviews (and derived classes
   * implementations) must be accounted for here, along with the separator
   * width from SeparatorEvenOddBufferTextCell. */
  static constexpr KDCoordinate k_calculationCellWidth = 7 * Poincare::PrintFloat::glyphLengthForFloatWithPrecision(Poincare::Preferences::VeryLargeNumberOfSignificantDigits) + 2 * Escher::EvenOddCell::k_horizontalMargin + Escher::EvenOddCell::k_separatorWidth;
  static constexpr KDCoordinate k_margin = 8;
  static constexpr KDCoordinate k_scrollBarMargin = Escher::Metric::CommonRightMargin;

  Escher::Responder * tabController() const override;
  Escher::SelectableTableView * selectableTableView() override { return &m_selectableTableView; }
  CalculationSelectableTableView m_selectableTableView;
  Shared::PrefacedTableView m_tableView;
  Shared::StoreTitleCell m_seriesTitleCells[k_numberOfSeriesTitleCells];
  Escher::EvenOddMessageTextCell m_calculationTitleCells[k_numberOfCalculationTitleCells];
  Escher::EvenOddMessageTextCell m_calculationSymbolCells[k_numberOfCalculationTitleCells];
  Shared::SeparatorEvenOddBufferTextCell m_calculationCells[k_numberOfCalculationCells];
  Shared::HideableEvenOddCell m_hideableCell[k_numberOfHeaderColumns];
  Store * m_store;
};

}


#endif
