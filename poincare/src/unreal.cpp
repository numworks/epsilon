#include <poincare/unreal.h>
#include <poincare/layout_helper.h>
#include <algorithm>

extern "C" {
#include <math.h>
#include <string.h>
}

namespace Poincare {

Layout UnrealNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::String(Unreal::Name(), Unreal::NameSize()-1);
}

int UnrealNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return std::min<int>(strlcpy(buffer, Unreal::Name(), bufferSize), bufferSize - 1);
}

}
