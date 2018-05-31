#ifndef REGRESSION_EVEN_ODD_DOUBLE_BUFFER_TEXT_CELL_WITH_SEPARATOR_H
#define REGRESSION_EVEN_ODD_DOUBLE_BUFFER_TEXT_CELL_WITH_SEPARATOR_H

#include <escher.h>

class EvenOddDoubleBufferTextCellWithSeparator : public EvenOddCell, public Responder{
public:
  EvenOddDoubleBufferTextCellWithSeparator(Responder * parentResponder = nullptr);
  const char * firstText();
  const char * secondText();
  void reloadCell() override;
  void setHighlighted(bool highlight) override;
  Responder * responder() override {
    return this;
  }
  void setEven(bool even) override;
  bool firstTextSelected();
  void selectFirstText(bool selectFirstText);
  void setFirstText(const char * textContent);
  void setSecondText(const char * textContent);
  void setTextColor(KDColor textColor);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  bool handleEvent(Ion::Events::Event event) override;
protected:
  bool m_firstTextSelected;
  EvenOddBufferTextCell m_firstBufferTextView;
  EvenOddBufferTextCell m_secondBufferTextView;
};

#endif
