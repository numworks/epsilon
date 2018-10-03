#include "storage_graph_controller_helper.h"
#include "../../constant.h"
#include "../../shared/poincare_helpers.h"

using namespace Shared;
using namespace Poincare;

namespace Graph {

bool StorageGraphControllerHelper::privateMoveCursorHorizontally(Shared::CurveViewCursor * cursor, int direction, Shared::InteractiveCurveViewRange * range, int numberOfStepsInGradUnit, StorageCartesianFunction * function, Shared::TextFieldDelegateApp * app, float cursorTopMarginRatio, float cursorRightMarginRatio, float cursorBottomMarginRatio, float cursorLeftMarginRatio) {
  double xCursorPosition = cursor->x();
  double x = direction > 0 ? xCursorPosition + range->xGridUnit()/numberOfStepsInGradUnit : xCursorPosition -  range->xGridUnit()/numberOfStepsInGradUnit;
  double y = function->evaluateAtAbscissa(x, app->localContext());
  cursor->moveTo(x, y);
  range->panToMakePointVisible(x, y, cursorTopMarginRatio, cursorRightMarginRatio, cursorBottomMarginRatio, cursorLeftMarginRatio);
  return true;
}

void StorageGraphControllerHelper::reloadDerivativeInBannerViewForCursorOnFunction(Shared::CurveViewCursor * cursor, StorageCartesianFunction * function, TextFieldDelegateApp * app) {
  char buffer[StorageFunctionBannerDelegate<StorageCartesianFunction>::k_maxNumberOfCharacters+PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  const char * space = "                  ";
  int spaceLength = strlen(space);
  const char * legend = "00(x)=";
  int legendLength = strlen(legend);
  int numberOfChar = strlcpy(buffer, legend, legendLength+1);
  buffer[0] = function->name()[0];
  buffer[1] = '\'';
  double y = function->approximateDerivative(cursor->x(), app->localContext());
  numberOfChar += PoincareHelpers::ConvertFloatToText<double>(y, buffer + legendLength, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits), Constant::ShortNumberOfSignificantDigits);
  strlcpy(buffer+numberOfChar, space, spaceLength+1);
  buffer[k_maxDigitLegendLength+6] = 0;
  bannerView()->setLegendAtIndex(buffer, 2);
}

}
