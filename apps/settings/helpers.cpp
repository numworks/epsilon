#include "helpers.h"
#include <poincare/layout_engine.h>
#include <ion/charset.h>
#include "../../poincare/src/layout/horizontal_layout.h"
#include "../../poincare/src/layout/vertical_offset_layout.h"

using namespace Poincare;

namespace Settings {
namespace Helpers {

ExpressionLayout * CartesianComplexFormat(KDText::FontSize fontSize) {
  const char text[] = {'a','+', Ion::Charset::IComplex, 'b', ' '};
  return LayoutEngine::createStringLayout(text, sizeof(text), fontSize);
}

ExpressionLayout * PolarComplexFormat(KDText::FontSize fontSize) {
  const char base[] = {'r', Ion::Charset::Exponential};
  const char superscript[] = {Ion::Charset::IComplex, Ion::Charset::SmallTheta, ' '};
  return new HorizontalLayout(
      LayoutEngine::createStringLayout(base, sizeof(base), fontSize),
      new VerticalOffsetLayout(LayoutEngine::createStringLayout(superscript, sizeof(superscript), fontSize), VerticalOffsetLayout::Type::Superscript, false),
      false);
}

}
}
