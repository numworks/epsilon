#ifndef POINCARE_JUNIOR_LAYOUTTER_H
#define POINCARE_JUNIOR_LAYOUTTER_H

#include <poincare_junior/src/expression/decimal.h>
#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/memory/tree_ref.h>

using Poincare::Preferences;

namespace PoincareJ {

class Layoutter {
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
  Layoutter(bool linearMode, bool addSeparators, int numberOfSignificantDigits,
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
}  // namespace PoincareJ

#endif
