#include <escher/header_view_delegate.h>
#include <assert.h>

HeaderViewDelegate::HeaderViewDelegate(HeaderViewController * headerViewController) :
  m_headerViewController(headerViewController)
{
}

int HeaderViewDelegate::numberOfButtons() const {
  return 0;
}

Button * HeaderViewDelegate::buttonAtIndex(int index) {
  assert(false);
  return nullptr;
}

HeaderViewController * HeaderViewDelegate::headerViewController() {
  return m_headerViewController;
}
