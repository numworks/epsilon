#ifndef SHARED_INTERACTIVE_CURVE_VIEW_DELEGATE_H
#define SHARED_INTERACTIVE_CURVE_VIEW_DELEGATE_H

#include <assert.h>
#include <poincare/context.h>
#include <poincare/range.h>

#include "function_store.h"

namespace Shared {

class InteractiveCurveViewRange;
class FunctionStore;

class InteractiveCurveViewRangeDelegate {
 public:
  constexpr static float k_defaultXHalfRange =
      Poincare::Range1D<float>::k_defaultHalfLength;

  static float DefaultAddMargin(float x, float range, bool isVertical,
                                bool isMin, float top, float bottom, float left,
                                float right);

  virtual uint64_t autoZoomChecksum() const {
    return (static_cast<uint64_t>(
                Ion::Storage::FileSystem::sharedFileSystem->checksum())
            << 32) +
           static_cast<uint64_t>(Poincare::Preferences::SharedPreferences()
                                     ->mathPreferencesCheckSum());
  }
  virtual Poincare::Range2D<float> optimalRange(
      bool computeX, bool computeY,
      Poincare::Range2D<float> originalRange) const = 0;
  virtual float addMargin(float x, float range, bool isVertical,
                          bool isMin) const = 0;
  Poincare::Range2D<float> addMargins(Poincare::Range2D<float> range) const;
  virtual void refreshCursorAfterComputingRange() = 0;
  virtual void updateZoomButtons() = 0;
  virtual void tidyModels(Poincare::PoolObject* treePoolCursor) = 0;
};

}  // namespace Shared

#endif
