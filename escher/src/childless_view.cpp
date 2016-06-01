#include <escher/childless_view.h>
extern "C" {
#include <assert.h>
}

int ChildlessView::numberOfSubviews() const {
  return 0;
}

View * ChildlessView::subview(int index) {
  assert(false);
  return nullptr;
}

void ChildlessView::layoutSubviews() {
}

void ChildlessView::storeSubviewAtIndex(View * v, int index) {
  assert(false);
}
