#ifndef CODE_DOUBLE_BUFFER_TEXT_CELL_H
#define CODE_DOUBLE_BUFFER_TEXT_CELL_H

#include <escher/table_cell.h>
#include <escher/buffer_text_view.h>

class DoubleBufferTextCell : public TableCell {
public:
  DoubleBufferTextCell(KDText::FontSize size = KDText::FontSize::Small, float horizontalAlignment = 0.0f, float verticalAlignment = 0.5f);
  const char * firstText();
  const char * secondText();
  void setFirstText(const char * textContent);
  void setSecondText(const char * textContent);
  void setFirstTextColor(KDColor textColor);
  void setSecondTextColor(KDColor textColor);

  /* TableCell */
  View * labelView() const override { return const_cast<View *>(static_cast<const View *>(&m_firstBufferTextView)); }
  View * accessoryView() const override { return const_cast<View *>(static_cast<const View *>(&m_secondBufferTextView)); }

  /* HighlightCell */
  void setHighlighted(bool highlight) override;

  /* View */
  int numberOfSubviews() const override { return 2; }
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
protected:
  BufferTextView m_firstBufferTextView;
  BufferTextView m_secondBufferTextView;
};

#endif
