#ifndef POINCARE_LAYOUTER_H
#define POINCARE_LAYOUTER_H

#include <omg/enums.h>
#include <poincare/src/expression/decimal.h>
#include <poincare/src/expression/integer.h>
#include <poincare/src/memory/tree_ref.h>

using Poincare::Preferences;

namespace Poincare::Internal {

class Layouter {
 public:
  // Eats expression to built its layout inplace.
  static Tree* LayoutExpression(Tree* expression, bool linearMode = false,
                                int numberOfSignificantDigits = -1,
                                Preferences::PrintFloatMode floatMode =
                                    Preferences::PrintFloatMode::Decimal,
                                OMG::Base base = OMG::Base::Decimal);

  static bool AddThousandsSeparators(Tree* rack);

  /* Recursively remove OperatorSeparators and ThousandsSeparators in rack
   * and replace UnitSeparators with a middle dot. */
  static void StripSeparators(Tree* rack);

 private:
  Layouter(bool linearMode, bool addSeparators, int numberOfSignificantDigits,
           Preferences::PrintFloatMode floatMode, OMG::Base base)
      : m_linearMode(linearMode),
        m_addSeparators(addSeparators),
        m_numberOfSignificantDigits(numberOfSignificantDigits),
        m_floatMode(floatMode),
        m_base(base) {}
  // Eats expression to built its layout inplace. May raise TreeStack exceptions
  static Tree* UnsafeLayoutExpression(Tree* expression, bool linearMode = false,
                                      int numberOfSignificantDigits = -1,
                                      Preferences::PrintFloatMode floatMode =
                                          Preferences::PrintFloatMode::Decimal,
                                      OMG::Base base = OMG::Base::Decimal);
  void addOperatorSeparator(Tree* layoutParent);
  void addUnitSeparator(Tree* layoutParent);
  bool requireSeparators(const Tree* expression);
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
  bool implicitAddition(const Tree* addition);
  void serializeDecimalOrFloat(const Tree* expression, char* buffer,
                               size_t bufferSize);
  // Recursively replace "+-" into "-" in rack
  static void StripUselessPlus(Tree* rack);
  bool m_linearMode;
  bool m_addSeparators;
  int m_numberOfSignificantDigits;
  Preferences::PrintFloatMode m_floatMode;
  OMG::Base m_base;
};
}  // namespace Poincare::Internal

#endif
