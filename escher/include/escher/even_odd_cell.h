#ifndef ESCHER_EVEN_ODD_CELL_H
#define ESCHER_EVEN_ODD_CELL_H

#include <escher/table_view_cell.h>

class EvenOddCell : public TableViewCell {
public:
  EvenOddCell();
  void setEven(bool even);
  virtual KDColor backgroundColor() const;
  void drawRect(KDContext * ctx, KDRect rect) const override;

  static constexpr KDColor k_evenLineBackgroundColor = KDColor::RGB24(0xF7F9FA);
  static constexpr KDColor k_oddLineBackgroundColor = KDColorWhite;
  static constexpr KDColor k_selectedLineBackgroundColor = KDColor::RGB24(0xC0D3EA);

protected:
  bool m_even;
};

#endif
