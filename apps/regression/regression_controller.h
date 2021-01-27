#ifndef REGRESSION_REGRESSION_CONTROLLER_H
#define REGRESSION_REGRESSION_CONTROLLER_H

#include <escher/message_table_cell_with_expression.h>
#include <escher/simple_list_view_data_source.h>
#include <escher/selectable_table_view.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/view_controller.h>
#include "store.h"
#include <apps/i18n.h>

namespace Regression {

class RegressionController : public Escher::ViewController, public Escher::SimpleListViewDataSource, public Escher::SelectableTableViewDataSource {
public:
  constexpr static KDCoordinate k_logisticCellHeight = 47;
  RegressionController(Escher::Responder * parentResponder, Store * store);
  void setSeries(int series) { m_series = series; }
  // ViewController
  const char * title() override;
  Escher::View * view() override { return &m_selectableTableView; }
  TELEMETRY_ID("Regression");

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  // SimpleListViewDataSource
  KDCoordinate nonMemoizedRowHeight(int j) override;
  KDCoordinate cellWidth() override { return m_selectableTableView.columnWidth(0); }
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override { return k_numberOfCells; }
  int numberOfRows() const override { return k_numberOfRows; }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
private:
  constexpr static int k_numberOfRows = 10;
  constexpr static int k_numberOfCells = 6; // (240 - 70) / 35
  Escher::MessageTableCellWithExpression m_regressionCells[k_numberOfCells];
  Escher::SelectableTableView m_selectableTableView;
  Store * m_store;
  int m_series;
};

}

#endif
