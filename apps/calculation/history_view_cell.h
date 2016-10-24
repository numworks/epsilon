#ifndef CALCULATION_HISTORY_VIEW_CELL_H
#define CALCULATION_HISTORY_VIEW_CELL_H

#include <escher.h>
#include "calculation.h"

namespace Calculation {

class HistoryViewCell : public View {
public:
  HistoryViewCell();
  BufferTextView * result();
  void setCalculation(Calculation * calculation);
  void setHighlighted(bool highlight);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
private:
  Calculation * m_calculation;
  bool m_highlighted;
  BufferTextView m_result;
};

}

#endif
