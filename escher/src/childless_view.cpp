#include <escher/childless_view.h>
extern "C" {
#include <assert.h>
}

int ChildlessView::numberOfSubviews() const {
  return 0;
}

const View * ChildlessView::subview(int index) const {
  assert(false);
  return nullptr;
}

void ChildlessView::layoutSubviews() {
}

void ChildlessView::storeSubviewAtIndex(View * v, int index) {
  assert(false);
}
