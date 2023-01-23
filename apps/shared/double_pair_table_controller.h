#ifndef SHARED_DOUBLE_PAIR_TABLE_CONTROLLER_H
#define SHARED_DOUBLE_PAIR_TABLE_CONTROLLER_H

#include "double_pair_store.h"
#include "prefaced_twice_table_view.h"
#include "tab_table_controller.h"
#include <escher/regular_table_view_data_source.h>
#include <escher/button_row_controller.h>
#include <escher/alternate_empty_view_controller.h>
#include <escher/tab_view_controller.h>
#include <apps/i18n.h>

namespace Shared {

class DoublePairTableController : public TabTableController, public Escher::RegularHeightTableViewDataSource, public Escher::ButtonRowDelegate, public Escher::AlternateEmptyViewDelegate, public Escher::SelectableTableViewDelegate, public PrefacedTableView::MarginDelegate {
public:
  constexpr static int k_titleNumberOfChars = 22;

  DoublePairTableController(Escher::Responder * parentResponder, Escher::ButtonRowController * header);

  // AlternateEmptyViewDelegate
  bool isEmpty() const override { return !store()->hasActiveSeries(); }
  I18n::Message emptyMessage() override { return I18n::Message::NoValueToCompute; }
  Escher::Responder * responderWhenEmpty() override;

  // ViewController
  const char * title() override { return I18n::translate(I18n::Message::StatTab); }
  Escher::View * view() override { return &m_prefacedTwiceTableView; }

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  // PrefacedTableView::MarginDelegate
  KDCoordinate columnPrefaceRightMargin() override;

  // SelectableTableViewDelegate
  bool canStoreContentOfCellAtLocation(Escher::SelectableTableView * t, int col, int row) const override { return row > 0 && col > 1; }

protected:
  constexpr static int k_maxNumberOfDisplayableRows = 11;
  constexpr static KDCoordinate k_cellHeight = Escher::Metric::SmallEditableCellHeight;
  constexpr static KDCoordinate k_margin = 8;
  constexpr static KDCoordinate k_scrollBarMargin = Escher::Metric::CommonRightMargin;

  // TableViewDataSource
  KDCoordinate defaultRowHeight() override { return k_cellHeight; }

  virtual DoublePairStore * store() const = 0;
  Escher::TabViewController * tabController() const override { return static_cast<Escher::TabViewController *>(parentResponder()->parentResponder()->parentResponder()); }
  Escher::SelectableTableView * selectableTableView() override { return &m_selectableTableView; }

  PrefacedTwiceTableView m_prefacedTwiceTableView;
  Escher::SelectableTableView m_selectableTableView;
};

}

#endif
