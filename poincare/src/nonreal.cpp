#include <poincare/nonreal.h>
#include <poincare/layout_helper.h>
#include <algorithm>

extern "C" {
#include <math.h>
#include <string.h>
}

namespace Poincare {

Layout NonrealNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::String(Nonreal::Name(), Nonreal::NameSize()-1);
}

int NonrealNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return std::min<int>(strlcpy(buffer, Nonreal::Name(), bufferSize), bufferSize - 1);
}

}