#ifndef SHARED_COLOR_CELL_CONTROLLER_H
#define SHARED_COLOR_CELL_CONTROLLER_H

#include <escher.h>
#include <apps/i18n.h>

namespace Shared {

class MessageTableCellWithColor : public MessageTableCell {
public:
  MessageTableCellWithColor();
  View * accessoryView() const override;
  void setColor(int i);
private:
  class ColorView : public TransparentView {
  public:
    ColorView();
    void setColor(int i) { m_index = i; }
    void drawRect(KDContext * ctx, KDRect rect) const override;
    KDSize minimalSizeForOptimalDisplay() const override;
    constexpr static KDCoordinate k_chevronHeight = 10;
    constexpr static KDCoordinate k_chevronWidth = 8;
  private:
    int m_index;
  };
  ColorView m_accessoryView;
  constexpr static I18n::Message k_textForIndex[] = {
    I18n::Message::PythonColorRed,
    I18n::Message::PythonColorBlue,
    I18n::Message::PythonColorGreen,
    I18n::Message::PythonColorYellow,
    I18n::Message::PythonColorPurple,
    I18n::Message::PythonColorBlue,
    I18n::Message::PythonColorPink,
    I18n::Message::PythonColorOrange
  };
};

}

#endif
