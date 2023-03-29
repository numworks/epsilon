#ifndef ESCHER_EVEN_ODD_MESSAGE_TEXT_CELL_H
#define ESCHER_EVEN_ODD_MESSAGE_TEXT_CELL_H

#include <escher/even_odd_cell.h>
#include <escher/i18n.h>
#include <escher/message_text_view.h>

namespace Escher {

class EvenOddMessageTextCell : public EvenOddCell {
 public:
  EvenOddMessageTextCell(KDGlyph::Format format = {
                             .horizontalAlignment = KDGlyph::k_alignCenter});
  void setMessage(I18n::Message textContent) {
    m_messageTextView.setMessage(textContent);
  }
  void setAlignment(float horizontalAlignment, float verticalAlignment);
  void setMessageFont(KDFont::Size font) { m_messageTextView.setFont(font); }
  void setTextColor(KDColor color) { m_messageTextView.setTextColor(color); }
  void setLeftMargin(KDCoordinate margin);

 protected:
  void updateSubviewsBackgroundAfterChangingState() override;
  int numberOfSubviews() const override;
  View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  MessageTextView m_messageTextView;
  KDCoordinate m_leftMargin;
};

}  // namespace Escher

#endif
