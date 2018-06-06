#ifndef REGRESSION_REGRESSION_CONTROLLER_H
#define REGRESSION_REGRESSION_CONTROLLER_H

#include "store.h"
#include <escher.h>
#include "../i18n.h"

namespace Regression {

class RegressionController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  RegressionController(Responder * parentResponder, Store * store);
  void setSeries(int series) { m_series = series; }
  // ViewController
  const char * title() override;
  View * view() override { return &m_selectableTableView; }

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  // SimpleListViewDataSource
  KDCoordinate cellHeight() override { return Metric::ParameterCellHeight; }
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override { return k_numberOfCells; }

  // TableViewDataSource
  int numberOfRows() override { return k_numberOfRows; }
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
private:
  constexpr static int k_numberOfRows = 9;
  constexpr static int k_numberOfCells = 6; // (240 - 70) / 35
  MessageTableCellWithExpression m_regressionCells[k_numberOfCells];
  //Poincare::ExpressionLayout * m_regressionLayouts[k_numberOfRows];
  SelectableTableView m_selectableTableView;
  Store * m_store;
  int m_series;
};

}

#endif
