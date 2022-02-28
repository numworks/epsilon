#ifndef ESCHER_BUFFER_TABLE_CELL_H
#define ESCHER_BUFFER_TABLE_CELL_H

#include <apps/i18n.h>
#include <escher/buffer_text_view.h>
#include <escher/table_cell.h>
#include <ion/display.h>

namespace Escher {

class BufferTableCell : public TableCell {
public:
  BufferTableCell();
  BufferTableCell(const KDFont * font, float horizontalAlignment, float verticalAlignment, KDColor textColor, KDColor backgroundColor, size_t maxNumberOfChars);
  const View * labelView() const override { return &m_labelView; }
  void setHighlighted(bool highlight) override;
  void setLabelText(const char * textBody);
  void setSimpleCustomLabelText(I18n::Message message, const char * string = "");
  void appendText(const char * textBody);
protected:
  BufferTextView m_labelView;
private:
  KDColor m_backgroundColor;
};

}

#endif
