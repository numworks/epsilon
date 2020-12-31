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
  int color() { return m_accessoryView.color(); }
  constexpr static I18n::Message k_textForIndex[Palette::numberOfDataColors()] = {
    I18n::Message::PythonColorRed,
    I18n::Message::PythonColorBlue,
    I18n::Message::PythonColorGreen,
    I18n::Message::PythonColorYellow,
    I18n::Message::PythonColorPurple,
    I18n::Message::PythonColorBlue,
    I18n::Message::PythonColorPink,
    I18n::Message::PythonColorOrange
  };
  class ColorView : public TransparentView {
  public:
    ColorView();
    void setColor(int i) { m_index = i; }
    int color() { return m_index; }
    void drawRect(KDContext * ctx, KDRect rect) const override;
    KDSize minimalSizeForOptimalDisplay() const override;
    constexpr static KDCoordinate k_colorSize = 12;
  private:
    int m_index;
  };
private:
  ColorView m_accessoryView;
};

}

#endif
