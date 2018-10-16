#ifndef GRAPH_LIST_TEXT_FIELD_FUNCTION_TITLE_CELL_H
#define GRAPH_LIST_TEXT_FIELD_FUNCTION_TITLE_CELL_H

#include "../../shared/function_title_cell.h"

namespace Graph {

class TextFieldFunctionTitleCell : public Shared::FunctionTitleCell {
public:
  TextFieldFunctionTitleCell(Orientation orientation = Orientation::VerticalIndicator, KDText::FontSize size = KDText::FontSize::Large) :
    Shared::FunctionTitleCell(orientation),
    m_textField(nullptr, m_textFieldBuffer, m_textFieldBuffer, k_textFieldBufferSize, nullptr, false, size, 0.5f, 0.5f)
  {}
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  void setColor(KDColor color) override;
  void setText(const char * textContent);
  KDText::FontSize fontSize() const override { return m_textField.fontSize(); }
  const char * text() const override {
    return m_textField.text();
  }
  int numberOfSubviews() const override { return 1; }
  View * subviewAtIndex(int index) override {
    assert(index == 0);
    return &m_textField;
  }
  void layoutSubviews() override;
protected:
  KDRect textFieldFrame() const;
private:
  constexpr static int k_textFieldBufferSize = TextField::maxBufferSize();
  TextField m_textField;
  char m_textFieldBuffer[k_textFieldBufferSize];
};

}

#endif
