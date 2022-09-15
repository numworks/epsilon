#ifndef ESCHER_ABSTRACT_BUTTON_CELL_H
#define ESCHER_ABSTRACT_BUTTON_CELL_H

#include <escher/highlight_cell.h>
#include <escher/i18n.h>
#include <escher/responder.h>
#include <escher/message_text_view.h>
#include <escher/invocation.h>
#include <escher/palette.h>

namespace Escher {

class AbstractButtonCell : public HighlightCell, public Responder {
public:
  AbstractButtonCell(Responder * parentResponder, I18n::Message textBody, Invocation invocation, KDFont::Size font = KDFont::Size::Small, KDColor textColor = KDColorBlack);
  void setMessage(I18n::Message message);
  bool handleEvent(Ion::Events::Event event) override;
  void setHighlighted(bool highlight) override;
  virtual KDColor highlightedBackgroundColor() const { return Palette::Select; }
  Responder * responder() override {
    return this;
  }
  KDSize minimalSizeForOptimalDisplay() const override;
protected:
  MessageTextView m_messageTextView;
  void layoutSubviews(bool force = false) override;
private:
  constexpr static KDCoordinate k_verticalMarginSmall = 5;
  constexpr static KDCoordinate k_verticalMarginLarge = 17;
  constexpr static KDCoordinate k_horizontalMarginSmall = 10;
  constexpr static KDCoordinate k_horizontalMarginLarge = 20;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  Invocation m_invocation;
  KDFont::Size m_font;
};

}

#endif
