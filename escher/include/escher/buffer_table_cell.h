#ifndef ESCHER_BUFFER_TABLE_CELL_H
#define ESCHER_BUFFER_TABLE_CELL_H

#include <escher/buffer_text_view.h>
#include <escher/table_cell.h>

namespace Escher {

class BufferTableCell : public TableCell {
public:
  BufferTableCell();
  const View * labelView() const override { return &m_labelView; }
  void setHighlighted(bool highlight) override;
  void setLabelText(const char * textBody);
  void appendText(const char * textBody);
  void setTextColor(KDColor color);
  void setMessageFont(const KDFont * font);
  void setBackgroundColor(KDColor color);
protected:
  KDColor backgroundColor() const override { return m_backgroundColor; }
private:
  BufferTextView m_labelView;
  KDColor m_backgroundColor;
};

}

#endif
