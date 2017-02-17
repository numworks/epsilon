#ifndef ESCHER_EXPRESSION_TABLE_CELL_WITH_POINTER_H
#define ESCHER_EXPRESSION_TABLE_CELL_WITH_POINTER_H

#include <escher/expression_table_cell.h>
#include <escher/pointer_text_view.h>

class ExpressionTableCellWithPointer : public ExpressionTableCell {
public:
  ExpressionTableCellWithPointer(char * accessoryText = nullptr, Layout layout = Layout::Horizontal);
  View * accessoryView() const override;
  void setHighlighted(bool highlight) override;
  void setAccessoryText(const char * textBody);
private:
  PointerTextView m_accessoryView;
};

#endif
