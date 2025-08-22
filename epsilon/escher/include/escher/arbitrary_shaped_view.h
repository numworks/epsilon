#pragma once

#include <escher/view.h>

namespace Escher {

/* This class is only a tag to remind its users that if a view is not
 * rectangular it should not mark itself dirty, nor have children (because
 * moving a child would leave a dirty spot).
 */

class ArbitraryShapedView : public View {
 private:
  /* Mark markRectAsDirty private to forbid the pattern
   * setState() {
   *  state = ...;
   *  markRectAsDirty(bounds());
   * } */
  using View::markAbsoluteRectAsDirty;
  using View::markRectAsDirty;
  using View::markWholeFrameAsDirty;
  int numberOfSubviews() const final override { return 0; }
};

}  // namespace Escher
