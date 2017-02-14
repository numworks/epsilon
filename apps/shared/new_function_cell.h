#ifndef SHARED_NEW_FUNCTION_CELL_H
#define SHARED_NEW_FUNCTION_CELL_H

#include <escher.h>

namespace Shared {

class NewFunctionCell : public EvenOddCell {
public:
  NewFunctionCell();
  void reloadCell() override;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
protected:
  virtual const char * text();
  PointerTextView m_pointerTextView;
};

}

#endif
