#include <poincare/expression.h>
#include <poincare/layout.h>
#include <poincare/old/symbol_abstract.h>
#include <poincare/src/layout/rack_layout.h>

namespace Poincare {

#define Layout OLayout

Layout Layout::clone() const {
  if (isUninitialized()) {
    return Layout();
  }
  PoolHandle c = PoolHandle::clone();
  Layout cast = Layout(static_cast<LayoutNode *>(c.object()));
  cast->invalidAllSizesPositionsAndBaselines();
  return cast;
}

}  // namespace Poincare
