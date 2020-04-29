#ifndef REGRESSION_REGRESSION_CONTROLLER_H
#define REGRESSION_REGRESSION_CONTROLLER_H

#include "store.h"
#include <escher.h>
#include <apps/i18n.h>

namespace Regression {

class RegressionController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource {
public:
  constexpr static KDCoordinate k_logisticCellHeight = 47;
  RegressionController(Responder * parentResponder, Store * store);
  void setSeries(int series) { m_series = series; }
  // ViewController
  const char * title() override;
  View * view() override { return &m_selectableTableView; }
  TELEMETRY_ID("Regression");

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  // ListViewDataSource
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override { return k_numberOfCells; }
  int typeAtLocation(int i, int j) override { return 0; }
  int numberOfRows() const override { return k_numberOfRows; }
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
private:
  constexpr static int k_numberOfRows = 10;
  constexpr static int k_numberOfCells = 6; // (240 - 70) / 35
  MessageTableCellWithExpression m_regressionCells[k_numberOfCells];
  SelectableTableView m_selectableTableView;
  Store * m_store;
  int m_series;
};

}

#endif
