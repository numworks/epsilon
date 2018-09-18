#include "helpers.h"
#include <poincare/layout_helper.h>
#include <ion/charset.h>

using namespace Poincare;

namespace Settings {
namespace Helpers {

Layout CartesianComplexFormat(KDText::FontSize fontSize) {
  const char text[] = {'a','+', Ion::Charset::IComplex, 'b', ' '};
  return LayoutHelper::String(text, sizeof(text), fontSize);
}

Layout PolarComplexFormat(KDText::FontSize fontSize) {
  const char base[] = {'r', Ion::Charset::Exponential};
  const char superscript[] = {Ion::Charset::IComplex, Ion::Charset::SmallTheta, ' '};
  return HorizontalLayout(
      LayoutHelper::String(base, sizeof(base), fontSize),
      VerticalOffsetLayout(LayoutHelper::String(superscript, sizeof(superscript), fontSize), VerticalOffsetLayoutNode::Type::Superscript)
     );
}

}
}
