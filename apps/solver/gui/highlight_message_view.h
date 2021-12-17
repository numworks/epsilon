#ifndef SOLVER_GUI_HIGHLIGHT_MESSAGE_VIEW_H
#define SOLVER_GUI_HIGHLIGHT_MESSAGE_VIEW_H

// TODO Hugo : Factorize with probability

#include <apps/i18n.h>
#include <escher/highlight_cell.h>
#include <escher/message_text_view.h>

namespace Solver {

/* Highlightable view with a message */
class HighlightMessageView : public Escher::HighlightCell {
public:
  void setHighlighted(bool highlighted) override {
    HighlightCell::setHighlighted(highlighted);
    m_messageView.setBackgroundColor(highlighted ? Escher::Palette::Select : KDColorWhite);
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
  Escher::MessageTextView m_messageView;
};

}  // namespace Solver

#endif /* SOLVER_GUI_HIGHLIGHT_MESSAGE_VIEW_H */
