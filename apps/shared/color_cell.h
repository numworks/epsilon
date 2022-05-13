#ifndef SHARED_COLOR_CELL_H
#define SHARED_COLOR_CELL_H

#include <escher/message_table_cell.h>
#include <escher/transparent_view.h>

namespace Shared {

class ColorCell : public Escher::MessageTableCell {
public:
  using MessageTableCell::MessageTableCell;
  const View * accessoryView() const override { return &m_colorView; }
  void setColor(KDColor color) { m_colorView.setColor(color); }
private:
  class ColorView : public Escher::TransparentView {
  public:
    void drawRect(KDContext * ctx, KDRect rect) const override;
    KDSize minimalSizeForOptimalDisplay() const override;
    void setColor(KDColor color);
  private:
    KDColor m_color;
  };
  ColorView m_colorView;
};

}

#endif
