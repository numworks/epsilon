#ifndef GRAPH_GRAPH_CONTROLLER_HELPER_H
#define GRAPH_GRAPH_CONTROLLER_HELPER_H

#include "../../shared/function_banner_delegate.h"
#include "../../shared/text_field_delegate_app.h"
#include "../../shared/interactive_curve_view_range.h"
#include "../cartesian_function_store.h"

namespace Graph {

class GraphControllerHelper {
protected:
  constexpr static int k_maxDigitLegendLength = 10;
  bool privateMoveCursorHorizontally(Shared::CurveViewCursor * cursor, int direction, Shared::InteractiveCurveViewRange * range, int numberOfStepsInGradUnit, Shared::Function * function, Shared::TextFieldDelegateApp * app, float cursorTopMarginRatio, float cursorRightMarginRatio, float cursorBottomMarginRatio, float cursorLeftMarginRatio);
  void reloadDerivativeInBannerViewForCursorOnFunction(Shared::CurveViewCursor * cursor, CartesianFunction * function, Shared::TextFieldDelegateApp * app);
  virtual Shared::BannerView * bannerView() = 0;
};

}

#endif
