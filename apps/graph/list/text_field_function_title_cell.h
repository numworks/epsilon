#ifndef GRAPH_LIST_TEXT_FIELD_FUNCTION_TITLE_CELL_H
#define GRAPH_LIST_TEXT_FIELD_FUNCTION_TITLE_CELL_H

#include <apps/shared/function_title_cell.h>
#include <apps/shared/function.h>
#include "text_field_with_max_length_and_extension.h"

namespace Graph {

class ListController;

class TextFieldFunctionTitleCell : public Shared::FunctionTitleCell, public Responder {
public:
  TextFieldFunctionTitleCell(ListController * listController, Orientation orientation = Orientation::VerticalIndicator, const KDFont * font = KDFont::ItalicLargeFont);
  TextField * textField() { return &m_textField; }
  void setEditing(bool editing);
  bool isEditing() const;
  void setHorizontalAlignment(float alignment);

  // FunctionTitleCell
  void setColor(KDColor color) override;
  const KDFont * font() const override { return m_textField.font(); }
  // EvenOddCell
  void setEven(bool even) override;
  // HighlightCell
  void setHighlighted(bool highlight) override;
  Responder * responder() override;
  const char * text() const override {
    return m_textField.text();
  }
  // View
  int numberOfSubviews() const override { return 1; }
  View * subviewAtIndex(int index) override {
    assert(index == 0);
    return &m_textField;
  }
  void layoutSubviews(bool force = false) override;
  void reloadCell() override;

  // Responder
  void didBecomeFirstResponder() override;
private:
  constexpr static KDCoordinate k_textFieldRightMargin = 4;
  constexpr static int k_textFieldBufferSize = Shared::Function::k_maxNameWithArgumentSize;
  float verticalAlignmentGivenExpressionBaselineAndRowHeight(KDCoordinate expressionBaseline, KDCoordinate rowHeight) const override;
  TextFieldWithMaxLengthAndExtension m_textField;
  char m_textFieldBuffer[k_textFieldBufferSize];
};

}

#endif
