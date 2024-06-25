#ifndef POINCARE_LAYOUTER_H
#define POINCARE_LAYOUTER_H

#include <poincare/src/expression/decimal.h>
#include <poincare/src/expression/integer.h>
#include <poincare/src/memory/tree_ref.h>

using Poincare::Preferences;

namespace Poincare::Internal {

class Layouter {
 public:
  static Tree* LayoutExpression(Tree* expression, bool linearMode = false,
                                int numberOfSignificantDigits = -1,
                                Preferences::PrintFloatMode floatMode =
                                    Preferences::PrintFloatMode::Decimal);

  static bool AddThousandSeparators(Tree* rack);

  // Recursively remove OperatorSeparators and ThousandSeparators in rack
  static void StripSeparators(Tree* rack);

  static bool ImplicitAddition(const Tree* addition);

 private:
  Layouter(bool linearMode, bool addSeparators, int numberOfSignificantDigits,
           Preferences::PrintFloatMode floatMode)
      : m_linearMode(linearMode),
        m_addSeparators(addSeparators),
        m_numberOfSignificantDigits(numberOfSignificantDigits),
        m_floatMode(floatMode) {}
  void addSeparator(Tree* layoutParent);
  bool requireSeparators(const Tree* expr);
  void layoutText(TreeRef& layoutParent, const char* text);
  void layoutBuiltin(TreeRef& layoutParent, Tree* expression);
  void layoutFunctionCall(TreeRef& layoutParent, Tree* expression,
                          const char* name);
  void layoutChildrenAsRacks(Tree* expression);
  void layoutIntegerHandler(TreeRef& layoutParent, IntegerHandler handler,
                            int decimalOffset = 0);
  void layoutInfixOperator(TreeRef& layoutParent, Tree* expression,
                           CodePoint op, bool multiplication = false);
  void layoutMatrix(TreeRef& layoutParent, Tree* expression);
  void layoutUnit(TreeRef& layoutParent, Tree* expression);
  void layoutPowerOrDivision(TreeRef& layoutParent, Tree* expression);
  void layoutExpression(TreeRef& layoutParent, Tree* expression,
                        int parentPriority);
  // Recursively replace "+-" into "-" in rack
  static void StripUselessPlus(Tree* rack);
  bool m_linearMode;
  bool m_addSeparators;
  int m_numberOfSignificantDigits;
  Preferences::PrintFloatMode m_floatMode;
};
}  // namespace Poincare::Internal

#endif
