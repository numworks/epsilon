#ifndef ESCHER_TRANSPARENT_VIEW_H
#define ESCHER_TRANSPARENT_VIEW_H

#include <escher/view.h>

namespace Escher {

/* If a non-rectangular view is always redrawn the same way (chevron) or at
 * least with the same shape (colorView), it is sufficient to make it arbitrary
 * shaped.
 */

class ArbitraryShapedView : public View {
  using View::markRectAsDirty;
  int numberOfSubviews() const final override { return 0; }
};

}  // namespace Escher
#endif
