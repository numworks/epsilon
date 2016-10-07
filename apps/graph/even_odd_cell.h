#ifndef GRAPH_EVEN_ODD_CELL_H
#define GRAPH_EVEN_ODD_CELL_H

#include <escher.h>

class EvenOddCell : public ChildlessView {
public:
  EvenOddCell(float horizontalAlignment, float verticalAlignment);
  void setEven(bool even);
  void setHighlighted(bool highlight);
  void reloadCell();
  KDColor backgroundColor() const;
  void drawRect(KDContext * ctx, KDRect rect) const override;

  static constexpr KDColor k_evenLineBackgroundColor = KDColor(0xF7F9FA);
  static constexpr KDColor k_oddLineBackgroundColor = KDColorWhite;
  static constexpr KDColor k_selectedLineBackgroundColor = KDColor(0xC0D3EA);

protected:
  float m_horizontalAlignment;
  float m_verticalAlignment;
  bool m_highlighted;
  bool m_even;
};

#endif
