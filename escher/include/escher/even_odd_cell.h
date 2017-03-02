#ifndef ESCHER_EVEN_ODD_CELL_H
#define ESCHER_EVEN_ODD_CELL_H

#include <escher/highlight_cell.h>

class EvenOddCell : public HighlightCell {
public:
  EvenOddCell();
  virtual void setEven(bool even);
  virtual KDColor backgroundColor() const;
  void drawRect(KDContext * ctx, KDRect rect) const override;
protected:
  bool m_even;
};

#endif
