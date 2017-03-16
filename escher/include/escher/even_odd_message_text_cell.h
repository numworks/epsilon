#ifndef ESCHER_EVEN_ODD_MESSAGE_TEXT_CELL_H
#define ESCHER_EVEN_ODD_MESSAGE_TEXT_CELL_H

#include <escher/even_odd_cell.h>
#include <escher/i18n.h>
#include <escher/message_text_view.h>

class EvenOddMessageTextCell : public EvenOddCell {
public:
  EvenOddMessageTextCell(KDText::FontSize size = KDText::FontSize::Large);
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  void setMessage(I18n::Message textContent, KDColor textColor = KDColorBlack);
  void setAlignment(float horizontalAlignment, float verticalAlignment);
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
protected:
  MessageTextView m_messageTextView;
};

#endif
