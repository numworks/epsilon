#ifndef REGRESSION_REGRESSION_CONTROLLER_H
#define REGRESSION_REGRESSION_CONTROLLER_H

#include <escher/message_table_cell_with_expression.h>
#include <escher/selectable_list_view_controller.h>
#include "model/model.h"
#include "store.h"
#include "overriden_title.h"
#include <apps/i18n.h>

namespace Regression {

class RegressionController : public Escher::SelectableListViewController<Escher::MemoizedListViewDataSource>, public OverridenTitle{
public:
  RegressionController(Escher::Responder * parentResponder, Store * store);
  void setSeries(int series) { m_series = series; }
  // ViewController
  const char * title() override { return getTitle(I18n::Message::RegressionModel); }
  TELEMETRY_ID("Regression");

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  // MemoizedListViewDataSource
  KDCoordinate nonMemoizedRowHeight(int j) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override { return k_numberOfCells; }
  int numberOfRows() const override { return k_numberOfRows; }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
private:
  // Model::Type::None isn't made available here
  constexpr static int k_numberOfRows = Model::k_numberOfModels - 1;
  constexpr static int k_numberOfCells = ((Ion::Display::Height - Escher::Metric::TitleBarHeight - Escher::Metric::TabHeight - 2*Escher::Metric::StackTitleHeight) / Escher::TableCell::k_minimalLargeFontCellHeight) + 2; // Remaining cell can be above and below so we add +2
  constexpr static Model::Type ModelTypeAtIndex(int index) { return static_cast<Model::Type>(index + 1); }

  Escher::MessageTableCellWithExpression m_regressionCells[k_numberOfCells];
  Store * m_store;
  int m_series;
};

}

#endif
