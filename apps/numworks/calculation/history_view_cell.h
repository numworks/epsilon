#ifndef CALCULATION_HISTORY_VIEW_CELL_H
#define CALCULATION_HISTORY_VIEW_CELL_H

#include <escher.h>
#include "calculation.h"
#include "scrollable_expression_view.h"

namespace Calculation {

class HistoryViewCell : public ::EvenOddCell, public Responder {
public:
  enum class SubviewType {
    Input,
    Output
  };
  HistoryViewCell(Responder * parentResponder);
  void reloadCell() override;
  KDColor backgroundColor() const override;
  void setCalculation(Calculation * calculation);
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  constexpr static KDCoordinate k_digitHorizontalMargin = 10;
  constexpr static KDCoordinate k_digitVerticalMargin = 5;
  SubviewType selectedSubviewType();
  void setSelectedSubviewType(HistoryViewCell::SubviewType subviewType);
private:
  constexpr static KDCoordinate k_resultWidth = 80;
  ScrollableExpressionView m_inputView;
  ScrollableExpressionView m_outputView;
  SubviewType m_selectedSubviewType;
};

}

#endif
