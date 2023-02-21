#ifndef ESCHER_TRANSPARENT_IMAGE_CELL_WITH_MESSAGE_H
#define ESCHER_TRANSPARENT_IMAGE_CELL_WITH_MESSAGE_H

#include <escher/i18n.h>
#include <escher/message_text_view.h>
#include <escher/table_cell.h>
#include <escher/transparent_image_view.h>

namespace Escher {

class TransparentImageCellWithMessage : public TableCell {
 public:
  TransparentImageCellWithMessage();
  const View* labelView() const override { return &m_icon; }
  const View* subLabelView() const override { return &m_messageTextView; }
  void setHighlighted(bool highlight) override;
  void setMessage(I18n::Message message);
  void setImage(const Image* image);

 private:
  bool shouldAlignSublabelRight() const override { return false; }
  TransparentImageView m_icon;
  MessageTextView m_messageTextView;
};

}  // namespace Escher

#endif
