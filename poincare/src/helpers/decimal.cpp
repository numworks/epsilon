#include <poincare/helpers/decimal.h>
#include <poincare/print_float.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/layout/parser.h>
#include <poincare/src/layout/rack_from_text.h>
#include <poincare/symbol_context.h>

namespace Poincare {

using namespace Internal;

Tree* DecimalBuilderFromDouble(double value) {
  // TODO: this is a workaround until we port old Decimal::Builder(double)
  char buffer[PrintFloat::k_maxFloatCharSize];
  PrintFloat::PrintFloat::ConvertFloatToText(
      value, buffer, PrintFloat::k_maxFloatCharSize,
      PrintFloat::k_maxFloatGlyphLength,
      PrintFloat::k_maxNumberOfSignificantDigits,
      Preferences::PrintFloatMode::Decimal);
  assert(buffer[0] != 0);
  Tree* result = RackFromText(buffer);
  assert(result);
  result->moveTreeOverTree(Parser::Parse(result, EmptySymbolContext{}));
  // expression is only made of numbers and simple nodes, no need for contextes.
  ProjectionContext context = {};
  Simplification::ProjectAndReduce(result, &context);
  return result;
}

};  // namespace Poincare
