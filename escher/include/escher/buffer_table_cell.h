#ifndef ESCHER_BUFFER_TABLE_CELL_H
#define ESCHER_BUFFER_TABLE_CELL_H

#include <escher/buffer_text_view.h>
#include <escher/i18n.h>
#include <escher/table_cell.h>
#include <ion/display.h>

namespace Escher {

class BufferTableCell : public TableCell {
 public:
  BufferTableCell();
  BufferTableCell(KDFont::Size font, float horizontalAlignment,
                  float verticalAlignment, KDColor textColor,
                  KDColor backgroundColor, size_t maxNumberOfChars);
  const View* labelView() const override { return &m_labelView; }
  void setHighlighted(bool highlight) override;
  void setLabelText(const char* textBody);
  void setMessageWithPlaceholders(I18n::Message message, ...);
  void appendText(const char* textBody);

 protected:
  BufferTextView m_labelView;

 private:
  KDColor m_backgroundColor;
};

}  // namespace Escher

#endif
