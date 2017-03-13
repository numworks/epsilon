#ifndef ESCHER_BUTTON_H
#define ESCHER_BUTTON_H

#include <escher/highlight_cell.h>
#include <escher/responder.h>
#include <escher/pointer_text_view.h>
#include <escher/invocation.h>

class Button : public HighlightCell, public Responder {
public:
  Button(Responder * parentResponder, const char * textBody, Invocation invocation, KDText::FontSize size = KDText::FontSize::Small, KDColor textColor = KDColorBlack);
  bool handleEvent(Ion::Events::Event event) override;
  void setHighlighted(bool highlight) override;
  KDSize minimalSizeForOptimalDisplay() const override;
protected:
  PointerTextView m_pointerTextView;
private:
  constexpr static KDCoordinate k_verticalMargin = 5;
  constexpr static KDCoordinate k_horizontalMargin = 10;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  Invocation m_invocation;
};

#endif
