#ifndef STATISTICS_CALCULATION_CONTROLLER_H
#define STATISTICS_CALCULATION_CONTROLLER_H

#include <escher/alternate_empty_view_controller.h>
#include <escher/button_row_controller.h>
#include <poincare/print_float.h>
#include <poincare/preferences.h>
#include "store.h"
#include "calculation_selectable_table_view.h"
#include "../shared/hideable_even_odd_cell.h"
#include "../shared/separator_even_odd_buffer_text_cell.h"
#include "../shared/store_title_cell.h"
#include "../shared/tab_table_controller.h"

namespace Statistics {

class CalculationController : public Shared::TabTableController, public Escher::ButtonRowDelegate, public Escher::TableViewDataSource, public Escher::AlternateEmptyViewDefaultDelegate {

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
  TELEMETRY_ID("Calculation");

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
private:
  static constexpr int k_totalNumberOfRows = 15;
  static constexpr int k_maxNumberOfDisplayableRows = 11;
  static constexpr int k_numberOfCalculationCells = 3 * k_maxNumberOfDisplayableRows;
  static constexpr int k_numberOfSeriesTitleCells = 3;
  static constexpr int k_numberOfCalculationTitleCells = k_maxNumberOfDisplayableRows;

  static constexpr int k_calculationTitleCellType = 0;
  static constexpr int k_calculationCellType = 1;
  static constexpr int k_seriesTitleCellType = 2;
  static constexpr int k_hideableCellType = 3;
  static constexpr KDCoordinate k_cellHeight = 20;
  static constexpr KDCoordinate k_calculationTitleCellWidth = 175;
  /* Margins from EvenOddCell::layoutSubviews (and derived classes
   * implementations) must be accounted for here.
   * TODO: change 7 for KDFont::SmallFont->glyphSize().width() */
  static constexpr KDCoordinate k_calculationCellWidth = 7*Poincare::PrintFloat::glyphLengthForFloatWithPrecision(Poincare::Preferences::LargeNumberOfSignificantDigits)+2*Escher::EvenOddCell::k_horizontalMargin;
  static constexpr KDCoordinate k_margin = 8;
  static constexpr KDCoordinate k_scrollBarMargin = Escher::Metric::CommonRightMargin;

  Escher::Responder * tabController() const override;
  Escher::SelectableTableView * selectableTableView() override { return &m_selectableTableView; }
  CalculationSelectableTableView m_selectableTableView;
  Shared::StoreTitleCell m_seriesTitleCells[k_numberOfSeriesTitleCells];
  Escher::EvenOddMessageTextCell m_calculationTitleCells[k_numberOfCalculationTitleCells];
  Shared::SeparatorEvenOddBufferTextCell m_calculationCells[k_numberOfCalculationCells];
  Shared::HideableEvenOddCell m_hideableCell;
  Store * m_store;
};

}


#endif
