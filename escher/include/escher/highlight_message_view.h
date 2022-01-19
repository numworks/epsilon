#ifndef ESCHER_HIGHLIGHT_MESSAGE_VIEW_H
#define ESCHER_HIGHLIGHT_MESSAGE_VIEW_H

#include <escher/i18n.h>
#include <escher/highlight_cell.h>
#include <escher/message_text_view.h>

namespace Escher {

/* Highlightable view with a message */
class HighlightMessageView : public HighlightCell {
public:
  void setHighlighted(bool highlighted) override {
    HighlightCell::setHighlighted(highlighted);
    m_messageView.setBackgroundColor(highlighted ? Palette::Select : KDColorWhite);
  }
  void setMessage(I18n::Message m) { m_messageView.setMessage(m); }
  int numberOfSubviews() const override { return 1; }
  View * subviewAtIndex(int i) override { return &m_messageView; }
  KDSize minimalSizeForOptimalDisplay() const override {
    return m_messageView.minimalSizeForOptimalDisplay();
  }
  void layoutSubviews(bool force) override { m_messageView.setFrame(bounds(), force); }
  void setFont(const KDFont * font) { m_messageView.setFont(font); }
  void setTextColor(KDColor color) { m_messageView.setTextColor(color); }

private:
  MessageTextView m_messageView;
};

}  // namespace Escher

#endif /* ESCHER_HIGHLIGHT_MESSAGE_VIEW_H */
