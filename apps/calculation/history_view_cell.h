#ifndef CALCULATION_HISTORY_VIEW_CELL_H
#define CALCULATION_HISTORY_VIEW_CELL_H

#include <escher.h>
#include "calculation.h"
#include "scrollable_expression_view.h"
#include "../shared/scrollable_exact_approximate_expressions_view.h"

namespace Calculation {

class HistoryViewCell;

class HistoryViewCellDataSource {
public:
  enum class SubviewType {
    Input,
    Output
  };
  HistoryViewCellDataSource();
  void setSelectedSubviewType(SubviewType subviewType);
  SubviewType selectedSubviewType() { return m_selectedSubviewType; }
private:
  /* This method should belong to a delegate instead of a data source but as
   * both the data source and the delegate will be the same controller, we
   * avoid keeping 2 pointers in HistoryViewCell. */
  // It returns the selected cell at the end of the method
  virtual HistoryViewCell * historyViewCellDidChangeSelection() = 0;
  SubviewType m_selectedSubviewType;
};

class HistoryViewCell : public ::EvenOddCell, public Responder {
public:
  HistoryViewCell(Responder * parentResponder = nullptr);
  void cellDidSelectSubview(HistoryViewCellDataSource::SubviewType type);
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  void setDataSource(HistoryViewCellDataSource * dataSource) { m_dataSource = dataSource; }
  Responder * responder() override {
    return this;
  }
  Poincare::Layout layout() const override;
  KDColor backgroundColor() const override;
  void setCalculation(Calculation * calculation, bool expanded = false);
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  Shared::ScrollableExactApproximateExpressionsView * outputView();
private:
  constexpr static KDCoordinate k_resultWidth = 80;
  void reloadScroll();
  void reloadOutputSelection();
  bool displayLeftLayout() const;
  uint32_t m_calculationCRC32;
  Calculation::DisplayOutput m_calculationDisplayOutput;
  bool m_calculationExpanded;
  ScrollableExpressionView m_inputView;
  Shared::ScrollableExactApproximateExpressionsView m_scrollableOutputView;
  HistoryViewCellDataSource * m_dataSource;
};

}

#endif
