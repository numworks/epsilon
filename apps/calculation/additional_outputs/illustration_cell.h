#ifndef CALCULATION_ADDITIONAL_OUTPUTS_ILLUSTRATION_CELL_H
#define CALCULATION_ADDITIONAL_OUTPUTS_ILLUSTRATION_CELL_H

#include <escher/bordered_cell.h>

namespace Calculation {

class IllustrationCell : public BorderedCell {
public:
  using BorderedCell::BorderedCell;
  void setHighlighted(bool highlight) override { return; }
private:
  int numberOfSubviews() const override { return 1; }
  View * subviewAtIndex(int index) override { return view(); }
  void layoutSubviews(bool force = false) override;
  virtual View * view() = 0;
};

}

#endif

