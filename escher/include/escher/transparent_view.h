#ifndef ESCHER_TRANSPARENT_VIEW_H
#define ESCHER_TRANSPARENT_VIEW_H

#include <escher/view.h>

namespace Escher {

/* If a non-rectangular view is always redrawn the same way (chevron) or at
 * least with the same shape (colorView), it is sufficient to make it arbitrary
 * shaped.
 *
 * If it has a state that can change and this changes its shape, then it must
 * be a TransparentView that knows its parent to be able to invalidate it.
 */

class ArbitraryShapedView : public View {
  using View::markRectAsDirty;
  int numberOfSubviews() const final override { return 0; }
};

class TransparentView : public ArbitraryShapedView {
 public:
  TransparentView(View* superview) : m_superview(superview) {
    assert(superview);
  }

 protected:
  /* This method does not need to be virtual since markRectAsDirty is called
   * only by classes on themselves (or by setChildFrame for parent views) */
  void markRectAsDirty(KDRect rect);

 private:
  View* m_superview;
};

}  // namespace Escher
#endif
