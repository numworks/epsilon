#ifndef SHARED_BUTTON_CELL_H
#define SHARED_BUTTON_CELL_H

#include <escher/highlight_cell.h>
#include <escher/i18n.h>
#include <escher/invocation.h>
#include <escher/message_text_view.h>
#include <escher/metric.h>
#include <escher/palette.h>
#include <escher/responder.h>

namespace Escher {

class ButtonCell : public HighlightCell, public Responder {
 public:
  enum class Style { None, EmbossedLight, EmbossedGray };

  ButtonCell(Responder* parentResponder, I18n::Message textBody,
             Escher::Invocation invocation, Style style = Style::None,
             KDFont::Size fontSize = KDFont::Size::Large,
             KDColor textColor = KDColorBlack);

  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void setHighlighted(bool highlight) override;
  Responder* responder() override { return this; }

  virtual KDColor highlightedBackgroundColor() const { return Palette::Select; }
  void setMessage(I18n::Message message) {
    m_messageTextView.setMessage(message);
  }

 protected:
  constexpr static KDCoordinate k_lineThickness =
      Escher::Metric::CellSeparatorThickness;

  MessageTextView m_messageTextView;

 private:
  // Small font without borders
  constexpr static KDCoordinate k_horizontalMarginSmallFontWithoutBorders = 10;
  constexpr static KDCoordinate k_verticalMarginSmallFontWithoutBorders = 5;
  // Small font with borders
  constexpr static KDCoordinate k_horizontalMarginSmallFontWithBorders = 13;
  constexpr static KDCoordinate k_verticalMarginSmallFontWithBorders = 13;
  // Large font with borders
  constexpr static KDCoordinate k_horizontalMarginLargeFontWithBorders = 20;
  constexpr static KDCoordinate k_verticalMarginLargeFontWithBorders = 17;

  int numberOfSubviews() const override { return 1; }
  View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  void drawBorder(KDContext* ctx, OMG::Direction direction, int index,
                  KDColor color) const;
  int numberOfBordersInDirection(OMG::Direction direction) const;

  Invocation m_invocation;
  KDFont::Size m_font;
  Style m_style;
};

class SimpleButtonCell : public ButtonCell {
 public:
  SimpleButtonCell(Responder* parentResponder, I18n::Message textBody,
                   Invocation invocation,
                   KDFont::Size font = KDFont::Size::Small,
                   KDColor textColor = KDColorBlack)
      : ButtonCell(parentResponder, textBody, invocation, Style::None, font,
                   textColor) {}
};

}  // namespace Escher
#endif
