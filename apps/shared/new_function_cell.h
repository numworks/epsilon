#ifndef SHARED_NEW_FUNCTION_CELL_H
#define SHARED_NEW_FUNCTION_CELL_H

#include <escher.h>

namespace Shared {

class NewFunctionCell : public EvenOddCell {
public:
  NewFunctionCell(I18n::Message text);
  void reloadCell() override;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
private:
  PointerTextView m_pointerTextView;
};

}

#endif
