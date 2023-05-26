#ifndef CALCULATION_ADDITIONAL_OUTPUTS_ILLUSTRATION_CELL_H
#define CALCULATION_ADDITIONAL_OUTPUTS_ILLUSTRATION_CELL_H

#include <escher/bordered.h>
#include <escher/highlight_cell.h>

namespace Calculation {

class IllustrationCell : public Escher::Bordered, public Escher::HighlightCell {
 public:
  void setHighlighted(bool highlight) override { return; }
  void drawRect(KDContext* ctx, KDRect rect) const override;

 private:
  int numberOfSubviews() const override { return 1; }
  View* subviewAtIndex(int index) override { return view(); }
  void layoutSubviews(bool force = false) override;
  virtual View* view() = 0;
};

}  // namespace Calculation

#endif
