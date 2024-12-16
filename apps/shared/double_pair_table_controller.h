#ifndef SHARED_DOUBLE_PAIR_TABLE_CONTROLLER_H
#define SHARED_DOUBLE_PAIR_TABLE_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/alternate_empty_view_controller.h>
#include <escher/button_row_controller.h>
#include <escher/even_odd_message_text_cell.h>
#include <escher/prefaced_twice_table_view.h>
#include <escher/regular_table_view_data_source.h>
#include <escher/solid_color_cell.h>
#include <escher/tab_view_controller.h>

#include "double_pair_store.h"
#include "tab_table_controller.h"

namespace Shared {

class DoublePairTableController
    : public TabTableController,
      public Escher::RegularHeightTableViewDataSource,
      public Escher::ButtonRowDelegate,
      public Escher::AlternateEmptyViewDelegate,
      public Escher::SelectableTableViewDelegate {
 public:
  DoublePairTableController(Escher::Responder* parentResponder,
                            Escher::ButtonRowController* header);

  // AlternateEmptyViewDelegate
  bool isEmpty() const override { return !store()->hasActiveSeries(); }
  I18n::Message emptyMessage() override {
    return I18n::Message::NoValueToCompute;
  }
  Escher::Responder* responderWhenEmpty() override;

  // ViewController
  Escher::View* view() override { return &m_prefacedTwiceTableView; }
  void viewWillAppear() override;

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  // RegularHeightTableViewDataSource
  bool canSelectCellAtLocation(int column, int row) override {
    return typeAtLocation(column, row) != k_hideableCellType;
  }
  bool canStoreCellAtLocation(int column, int row) override {
    return row > 0 && column > 1;
  }

  // TableViewDataSource
  int numberOfColumns() const override {
    return 2 + store()->numberOfActiveSeries();
  }
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) const override;
  int typeAtLocation(int column, int row) const override;
  KDCoordinate separatorBeforeColumn(int index) override;

 protected:
  // Number of cells
  constexpr static int k_maxNumberOfDisplayableRows = 11;
  constexpr static int k_numberOfHeaderColumns = 2;
  // Reusable cells. At most 4 columns can appear on the same screen.
  constexpr static int k_numberOfSeriesTitleCells = 4;
  constexpr static int k_numberOfCalculationCells =
      k_numberOfSeriesTitleCells * k_maxNumberOfDisplayableRows;
  // Cell sizes
  constexpr static KDCoordinate k_cellHeight =
      Escher::Metric::SmallEditableCellHeight;
  constexpr static int k_titleNumberOfChars = 22;
  constexpr static KDCoordinate k_calculationTitleCellWidth =
      Escher::Metric::SmallFontCellWidth(
          k_titleNumberOfChars, Escher::Metric::CellVerticalElementMargin);
  // Cell types
  constexpr static int k_hideableCellType = 0;
  constexpr static int k_seriesTitleCellType = 1;
  constexpr static int k_calculationTitleCellType = 2;
  constexpr static int k_calculationSymbolCellType = 3;
  constexpr static int k_calculationCellType = 4;

  // TableViewDataSource
  KDCoordinate defaultRowHeight() override { return k_cellHeight; }

  virtual DoublePairStore* store() const = 0;
  Escher::TabViewController* tabController() const override {
    return static_cast<Escher::TabViewController*>(
        parentResponder()->parentResponder()->parentResponder());
  }
  Escher::SelectableTableView* selectableTableView() override {
    return &m_selectableTableView;
  }

  Escher::PrefacedTwiceTableView m_prefacedTwiceTableView;
  Escher::SelectableTableView m_selectableTableView;

  Escher::SolidColorCell m_hideableCell[k_numberOfHeaderColumns];
  Escher::EvenOddMessageTextCell
      m_calculationTitleCells[k_maxNumberOfDisplayableRows];
  Escher::EvenOddMessageTextCell
      m_calculationSymbolCells[k_maxNumberOfDisplayableRows];
};

}  // namespace Shared

#endif
