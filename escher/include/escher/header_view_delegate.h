#ifndef ESCHER_HEADER_VIEW_DELEGATE_H
#define ESCHER_HEADER_VIEW_DELEGATE_H

#include <escher/button.h>

class HeaderViewController;

class HeaderViewDelegate {
public:
  HeaderViewDelegate(HeaderViewController * headerViewController);
  virtual int numberOfButtons() const;
  virtual Button * buttonAtIndex(int index);
  HeaderViewController * headerViewController();
private:
  HeaderViewController * m_headerViewController;
};

#endif
