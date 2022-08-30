#ifndef ESCHER_MESSAGE_TABLE_CELL_H
#define ESCHER_MESSAGE_TABLE_CELL_H

#include <escher/message_text_view.h>
#include <escher/i18n.h>
#include <escher/table_cell.h>

namespace Escher {

class MessageTableCell : public TableCell {
public:
  MessageTableCell(I18n::Message label = (I18n::Message)0);
  const View * labelView() const override { return &m_messageTextView; }
  void setHighlighted(bool highlight) override;
  void setMessage(I18n::Message message);
  void setTextColor(KDColor color);
  void setMessageFont(KDFont::Size font);
  void setBackgroundColor(KDColor color);

  static bool ShouldEnterOnEvent(Ion::Events::Event event) {
    return event == Ion::Events::OK || event == Ion::Events::EXE;
  }
protected:
  KDColor backgroundColor() const override { return m_backgroundColor; }
private:
  MessageTextView m_messageTextView;
  KDColor m_backgroundColor;
};

}

#endif
