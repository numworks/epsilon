#ifndef SHARED_FUNCTION_BANNER_DELEGATE_H
#define SHARED_FUNCTION_BANNER_DELEGATE_H

#include "function_store.h"
#include "xy_banner_view.h"
#include "curve_view_cursor.h"
#include <poincare/preferences.h>

namespace Shared {

class FunctionBannerDelegate  {
public:
  /* Longest text that needs to be buffered is the ordinate of a parametric
   * function, f(t)=(...;...) */
  constexpr static size_t k_textBufferSize = 8 + 2 * Poincare::PrintFloat::charSizeForFloatsWithPrecision(Poincare::PrintFloat::k_numberOfStoredSignificantDigits);
  /* GetValueDisplayedOnBanner returns the value of t as displayed in the
   * banner, unless the difference from t exceeds deltaThreshold. If so,
   * return t. For instance, when a function is plotted between 1.000001 and
   * 1.000003, and the user goes to x = 1.000002, a small deltaThreshold
   * prevents him from being sent to x = 1
   * Note : Due to double encoding, not all values of t can be properly rounded.
   * For instance, x displayed as 0.01 can at best be encoded to x=0.010...02 */
  static double GetValueDisplayedOnBanner(double t, Poincare::Context * context, int significantDigits, double deltaThreshold, bool roundToZero = false);
protected:
  constexpr static int k_cappedNumberOfSignificantDigits = Poincare::Preferences::VeryLargeNumberOfSignificantDigits;
  virtual void reloadBannerViewForCursorOnFunction(CurveViewCursor * cursor, Ion::Storage::Record record, FunctionStore * functionStore, Poincare::Context * context, bool cappedNumberOfSignificantDigits = false);
  virtual XYBannerView * bannerView() = 0;
  virtual int numberOfSignificantDigits(bool capped = false) const {
    int userDigits = Poincare::Preferences::sharedPreferences->numberOfSignificantDigits();
    return capped && userDigits > k_cappedNumberOfSignificantDigits ? k_cappedNumberOfSignificantDigits : userDigits;
  }
};

}

#endif
