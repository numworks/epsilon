#include <poincare/letter_a_with_sub_and_superscript_layout.h>
#include <poincare/permute_coefficient.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

int LetterAWithSubAndSuperscriptLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, PermuteCoefficient::s_functionHelper.aliasesList().mainAlias(), SerializationHelper::TypeOfParenthesis::System);
}

void LetterAWithSubAndSuperscriptLayoutNode::renderLetter(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  /* Given that the A shape is closer to the subscript than the superscript, we
   * make the right margin one pixel larger to use the space evenly */
  KDCoordinate leftMargin = k_margin - 1;
  KDPoint base(p.x() + leftMargin, p.y() + k_symbolHeight);
  KDPoint slashTop(base.x() + k_symbolWidth/2, p.y());
  KDPoint slashBottom = base;
  ctx->drawAntialiasedLine(slashTop, slashBottom, expressionColor, backgroundColor);
  KDPoint antiSlashTop(base.x() + k_symbolWidth/2 + 1, p.y());
  KDPoint antiSlashBottom(base.x() + k_symbolWidth, base.y());
  ctx->drawAntialiasedLine(antiSlashTop, antiSlashBottom, expressionColor, backgroundColor);
  KDCoordinate mBar = 2;
  KDCoordinate yBar = p.y() + k_symbolHeight - k_barHeight;
  ctx->drawLine(KDPoint(base.x() + mBar, yBar), KDPoint(base.x() + k_symbolWidth - mBar, yBar), expressionColor);
}

}
