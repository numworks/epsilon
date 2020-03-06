#ifndef ESCHER_BUTTON_H
#define ESCHER_BUTTON_H

#include <escher/highlight_cell.h>
#include <escher/i18n.h>
#include <escher/responder.h>
#include <escher/message_text_view.h>
#include <escher/invocation.h>
#include <escher/palette.h>

class Button : public HighlightCell, public Responder {
public:
  Button(Responder * parentResponder, I18n::Message textBody, Invocation invocation, const KDFont * font = KDFont::SmallFont, KDColor textColor = KDColorBlack);
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
private:
  constexpr static KDCoordinate k_verticalMargin = 5;
  constexpr static KDCoordinate k_horizontalMarginSmall = 10;
  constexpr static KDCoordinate k_horizontalMarginLarge = 20;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  Invocation m_invocation;
  const KDFont * m_font;
};

#endif
