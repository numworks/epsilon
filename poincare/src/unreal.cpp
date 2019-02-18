#include <poincare/unreal.h>
#include <poincare/layout_helper.h>

extern "C" {
#include <math.h>
#include <string.h>
}

namespace Poincare {

Layout UnrealNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::String(Unreal::Name(), Unreal::NameSize()-1);
}

int UnrealNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  strlcpy(buffer, Unreal::Name(), bufferSize);
  return min(Unreal::NameSize(), bufferSize) - 1;
}

Unreal Unreal::Builder() {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(UnrealNode));
  UnrealNode * node = new (bufferNode) UnrealNode();
  TreeHandle h = TreeHandle::BuildWithBasicChildren(node);
  return static_cast<Unreal &>(h);
}

}
