#ifndef REGRESSION_EVEN_ODD_DOUBLE_BUFFER_TEXT_CELL_H
#define REGRESSION_EVEN_ODD_DOUBLE_BUFFER_TEXT_CELL_H

#include <escher.h>

class EvenOddDoubleBufferTextCell final : public EvenOddCell, public Responder{
public:
  EvenOddDoubleBufferTextCell(Responder * parentResponder = nullptr);
  const char * firstText() {
    return m_firstBufferTextView.text();
  }
  const char * secondText() {
    return m_secondBufferTextView.text();
  }
  void reloadCell() override;
  void setHighlighted(bool highlight) override;
  Responder * responder() override {
    return this;
  }
  void setEven(bool even) override;
  bool firstTextSelected() {
    return m_firstTextSelected;
  }
  void selectFirstText(bool selectFirstText);
  void setFirstText(const char * textContent) {
    m_firstBufferTextView.setText(textContent);
  }
  void setSecondText(const char * textContent) {
    m_secondBufferTextView.setText(textContent);
  }
  void setTextColor(KDColor textColor);
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
