#ifndef CALCULATION_HISTORY_VIEW_CELL_H
#define CALCULATION_HISTORY_VIEW_CELL_H

#include <escher.h>
#include "calculation.h"
#include "pretty_print_view.h"

namespace Calculation {

class HistoryViewCell : public TableViewCell, public Responder {
public:
  HistoryViewCell();
  void reloadCell() override;
  BufferTextView * result();
  void setCalculation(Calculation * calculation);
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
private:
  PrettyPrintView m_prettyPrint;
  BufferTextView m_result;
};

}

#endif
