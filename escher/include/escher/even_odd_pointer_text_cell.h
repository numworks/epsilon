#ifndef ESCHER_EVEN_ODD_POINTER_TEXT_CELL_H
#define ESCHER_EVEN_ODD_POINTER_TEXT_CELL_H

#include <escher/even_odd_cell.h>
#include <escher/pointer_text_view.h>

class EvenOddPointerTextCell : public EvenOddCell {
public:
  EvenOddPointerTextCell(KDText::FontSize size = KDText::FontSize::Large);
  void reloadCell() override;
  void setText(const char * textContent, KDColor textColor = KDColorBlack);
  void setAlignment(float horizontalAlignment, float verticalAlignment);
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
protected:
  PointerTextView m_pointerTextView;
};

#endif
