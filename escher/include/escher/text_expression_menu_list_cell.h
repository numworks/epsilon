#ifndef ESCHER_TEXT_EXPRESSION_MENU_LIST_CELL_H
#define ESCHER_TEXT_EXPRESSION_MENU_LIST_CELL_H

#include <escher/expression_menu_list_cell.h>
#include <escher/pointer_text_view.h>

class TextExpressionMenuListCell : public ExpressionMenuListCell {
public:
  TextExpressionMenuListCell(char * accessoryText = nullptr);
  void setHighlighted(bool highlight) override;
  void setAccessoryText(const char * textBody);
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  PointerTextView m_accessoryView;
};

#endif
