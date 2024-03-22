#ifndef POINCARE_JUNIOR_LAYOUTTER_H
#define POINCARE_JUNIOR_LAYOUTTER_H

#include <poincare_junior/src/expression/decimal.h>
#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/memory/edition_reference.h>

using Poincare::Preferences;

namespace PoincareJ {

class Layoutter {
 public:
  static Tree* LayoutExpression(Tree* expression, bool linearMode = false,
                                int numberOfSignificantDigits = -1,
                                Preferences::PrintFloatMode floatMode =
                                    Preferences::PrintFloatMode::Decimal);

  static bool AddThousandSeparators(Tree* rack);

  /* Remove OperatorSeparators and ThousandSeparators in rack */
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
  void layoutText(EditionReference& layoutParent, const char* text);
  void layoutBuiltin(EditionReference& layoutParent, Tree* expression);
  void layoutFunctionCall(EditionReference& layoutParent, Tree* expression,
                          const char* name);
  void layoutChildrenAsRacks(EditionReference& layoutParent, Tree* expression);
  void layoutIntegerHandler(EditionReference& layoutParent,
                            IntegerHandler handler, int decimalOffset = 0);
  void layoutInfixOperator(EditionReference& layoutParent, Tree* expression,
                           CodePoint op, bool multiplication = false);
  void layoutMatrix(EditionReference& layoutParent, Tree* expression);
  void layoutUnit(EditionReference& layoutParent, Tree* expression);
  void layoutPowerOrDivision(EditionReference& layoutParent, Tree* expression);
  void layoutExpression(EditionReference& layoutParent, Tree* expression,
                        int parentPriority);
  bool m_linearMode;
  bool m_addSeparators;
  int m_numberOfSignificantDigits;
  Preferences::PrintFloatMode m_floatMode;
};
}  // namespace PoincareJ

#endif
