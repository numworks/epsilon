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
      public Escher::SelectableTableViewDelegate,
      public Escher::PrefacedTableView::MarginDelegate {
 public:
  constexpr static int k_titleNumberOfChars = 22;

  DoublePairTableController(Escher::Responder *parentResponder,
                            Escher::ButtonRowController *header);

  // AlternateEmptyViewDelegate
  bool isEmpty() const override { return !store()->hasActiveSeries(); }
  I18n::Message emptyMessage() override {
    return I18n::Message::NoValueToCompute;
  }
  Escher::Responder *responderWhenEmpty() override;

  // ViewController
  Escher::View *view() override { return &m_prefacedTwiceTableView; }

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  // PrefacedTableView::MarginDelegate
  KDCoordinate columnPrefaceRightMargin() override;

  // RegularHeightTableViewDataSource
  bool canSelectCellAtLocation(int column, int row) override {
    return row > 0 || column > 1;
  }
  bool canStoreCellAtLocation(int column, int row) override {
    return row > 0 && column > 1;
  }

 protected:
  constexpr static int k_maxNumberOfDisplayableRows = 11;
  constexpr static KDCoordinate k_cellHeight =
      Escher::Metric::SmallEditableCellHeight;
  constexpr static KDCoordinate k_margin = 8;
  constexpr static KDCoordinate k_scrollBarMargin =
      Escher::Metric::CommonRightMargin;
  // Title & Symbol
  constexpr static int k_numberOfHeaderColumns = 2;

  // TableViewDataSource
  KDCoordinate defaultRowHeight() override { return k_cellHeight; }

  virtual DoublePairStore *store() const = 0;
  Escher::TabViewController *tabController() const override {
    return static_cast<Escher::TabViewController *>(
        parentResponder()->parentResponder()->parentResponder());
  }
  Escher::SelectableTableView *selectableTableView() override {
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
