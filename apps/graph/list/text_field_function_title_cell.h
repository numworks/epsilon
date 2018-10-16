#ifndef GRAPH_LIST_TEXT_FIELD_FUNCTION_TITLE_CELL_H
#define GRAPH_LIST_TEXT_FIELD_FUNCTION_TITLE_CELL_H

#include <apps/shared/function_title_cell.h>

namespace Graph {

class StorageListController;

class TextFieldFunctionTitleCell : public Shared::FunctionTitleCell, public Responder {
public:
  TextFieldFunctionTitleCell(StorageListController * listController, Orientation orientation = Orientation::VerticalIndicator, KDText::FontSize size = KDText::FontSize::Large);
  void setEditing(bool editing);
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
