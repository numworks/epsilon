#include "helpers.h"
#include <poincare/layout_helper.h>
#include <ion/charset.h>

using namespace Poincare;

namespace Settings {
namespace Helpers {

LayoutReference CartesianComplexFormat(KDText::FontSize fontSize) {
  const char text[] = {'a','+', Ion::Charset::IComplex, 'b', ' '};
  return LayoutHelper::String(text, sizeof(text), fontSize);
}

LayoutReference PolarComplexFormat(KDText::FontSize fontSize) {
  const char base[] = {'r', Ion::Charset::Exponential};
  const char superscript[] = {Ion::Charset::IComplex, Ion::Charset::SmallTheta, ' '};
  return HorizontalLayoutRef(
      LayoutHelper::String(base, sizeof(base), fontSize),
      VerticalOffsetLayoutRef(LayoutHelper::String(superscript, sizeof(superscript), fontSize), VerticalOffsetLayoutNode::Type::Superscript)
     );
}

}
}
