#ifndef ESCHER_EXPRESSION_TABLE_CELL_WITH_BUFFER_H
#define ESCHER_EXPRESSION_TABLE_CELL_WITH_BUFFER_H

#include <escher/buffer_text_view.h>
#include <escher/expression_table_cell.h>

namespace Escher {

class ExpressionTableCellWithBuffer : public ExpressionTableCell {
 public:
  ExpressionTableCellWithBuffer(Responder* responder = nullptr);
  const View* subLabelView() const override { return &m_subLabelView; }
  void setHighlighted(bool highlight) override;
  void setSubLabelText(const char* textBody);
  const char* subLabelText();

 private:
  BufferTextView m_subLabelView;
};

}  // namespace Escher

#endif
