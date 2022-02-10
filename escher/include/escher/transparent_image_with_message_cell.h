#ifndef ESCHER_TRANSPARENT_IMAGE_WITH_MESSAGE_CELL_H
#define ESCHER_TRANSPARENT_IMAGE_WITH_MESSAGE_CELL_H

#include <escher/message_text_view.h>
#include <escher/transparent_image_view.h>
#include <escher/i18n.h>
#include <escher/table_cell.h>

namespace Escher {

class TransparentImageWithMessageCell : public TableCell {
public:
  TransparentImageWithMessageCell();
  const View * labelView() const override { return &m_icon; }
  const View * subLabelView() const override { return &m_messageTextView; }
  void setHighlighted(bool highlight) override;
  void setMessage(I18n::Message message);
  void setImage(const Image * image);
private:
  bool shouldAlignSublabelRight() const override { return false; }
  TransparentImageView m_icon;
  MessageTextView m_messageTextView;
};

}

#endif
