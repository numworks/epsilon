#ifndef CALCULATION_HISTORY_VIEW_CELL_H
#define CALCULATION_HISTORY_VIEW_CELL_H

#include <escher.h>
#include "calculation.h"
#include "scrollable_expression_view.h"
#include "scrollable_output_expressions_view.h"

namespace Calculation {

class HistoryViewCell : public ::EvenOddCell, public Responder {
public:
  enum class SubviewType {
    Input,
    Output
  };
  HistoryViewCell(Responder * parentResponder);
  void reloadCell() override;
  void reloadScroll();
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  Responder * responder() override {
    return this;
  }
  KDColor backgroundColor() const override;
  void setCalculation(Calculation * calculation);
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  constexpr static KDCoordinate k_digitVerticalMargin = 5;
  SubviewType selectedSubviewType();
  void setSelectedSubviewType(HistoryViewCell::SubviewType subviewType);
  OutputExpressionsView * outputView();
private:
  constexpr static KDCoordinate k_resultWidth = 80;
  ScrollableExpressionView m_inputView;
  ScrollableOutputExpressionsView m_scrollableOutputView;
  SubviewType m_selectedSubviewType;
};

}

#endif
