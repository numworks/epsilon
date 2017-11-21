#ifndef SHARED_NEW_FUNCTION_CELL_H
#define SHARED_NEW_FUNCTION_CELL_H

#include <escher.h>

namespace Shared {

class NewFunctionCell : public EvenOddCell {
public:
  NewFunctionCell(I18n::Message text);
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
private:
  MessageTextView m_messageTextView;
};

}

#endif
