#ifndef ESCHER_EVEN_ODD_MESSAGE_TEXT_CELL_H
#define ESCHER_EVEN_ODD_MESSAGE_TEXT_CELL_H

#include <escher/even_odd_cell.h>
#include <escher/i18n.h>
#include <escher/metric.h>
#include <escher/message_text_view.h>

class EvenOddMessageTextCell : public EvenOddCell {
public:
  EvenOddMessageTextCell(const KDFont * font = KDFont::LargeFont);
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  void setMessage(I18n::Message textContent, KDColor textColor = KDColorBlack);
  void setAlignment(float horizontalAlignment, float verticalAlignment);
  void setMessageFont(const KDFont * font) { m_messageTextView.setFont(font); }
protected:
  constexpr static KDCoordinate k_horizontalMargin = Metric::CellMargin;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  MessageTextView m_messageTextView;
};

#endif
