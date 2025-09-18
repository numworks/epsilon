#pragma once

#include <omg/enums.h>
#include <poincare/print_float.h>
#include <poincare/src/expression/decimal.h>
#include <poincare/src/expression/integer.h>
#include <poincare/src/memory/tree_ref.h>
#include <poincare/symbol_context.h>

#include <string_view>

using Poincare::Preferences;

namespace Poincare::Internal {

enum class LayouterMode {
  // 2D layouts
  Natural,
  // 1D layouts only
  Linear,
  // 1D layouts without parentheses to preserve the 2D structure
  // (a×b)/c and a×(b/c) are layouted as a×b/c
  LinearCompact
};

struct LayouterParameters {
#if POINCARE_UNIT
  // Only used to decide whether a unit should be layoutted with '_' prefix
  const SymbolContext& symbolContext = k_emptySymbolContext;
#endif
  LayouterMode layouterMode;
  int numberOfSignificantDigits =
      PrintFloat::k_undefinedNumberOfSignificantDigits;
  Preferences::PrintFloatMode floatMode = Preferences::PrintFloatMode::Decimal;
  OMG::Base base = OMG::Base::Decimal;
};

class Layouter {
 public:
  // Eats expression to built its layout inplace.
  static Tree* LayoutExpression(Tree* expression,
                                LayouterParameters params = {});

  static bool AddThousandsSeparators(Tree* rack);

  /* Recursively remove OperatorSeparators, UnitSeparators and
   * ThousandsSeparators in rack */
  static void StripSeparators(Tree* rack);

 private:
  Layouter(LayouterParameters params)
      : m_parameters(params), m_addSeparators(false) {}

  // Eats expression to built its layout inplace. May raise TreeStack exceptions
  static Tree* UnsafeLayoutExpression(Tree* expression,
                                      LayouterParameters params = {});

  // Insert layout or codepoint depending on linear mode.
  Tree* insertParenthesis(TreeRef& layoutParent, bool isOpening,
                          bool isCurlyBrace = false);
  /* TODO: addOperatorSeparator, addUnitSeparator and AddThousandsSeparators
   * should all be applicable in linear mode. Separators should be stripped
   * before parsing layouts in 1D. */
  void addOperatorSeparator(Tree* layoutParent);
  void addUnitSeparator(Tree* layoutParent);
  bool requireSeparators(const Tree* expression);
  void layoutText(TreeRef& layoutParent, std::string_view text);
  void layoutBuiltin(TreeRef& layoutParent, Tree* expression);
  void layoutFunctionCall(TreeRef& layoutParent, Tree* expression,
                          std::string_view name);
  void layoutChildrenAsRacks(Tree* expression);
  void layoutIntegerHandler(TreeRef& layoutParent, IntegerHandler handler,
                            int decimalOffset = 0);
  void layoutInfixOperator(TreeRef& layoutParent, Tree* expression,
                           CodePoint op, bool multiplication = false);
  void layoutMatrix(TreeRef& layoutParent, Tree* expression);
  void layoutSequence(TreeRef& layoutParent, Tree* expression);
  void layoutUnit(TreeRef& layoutParent, Tree* expression);
  void layoutPowerOrDivision(TreeRef& layoutParent, Tree* expression);
  void layoutExpression(TreeRef& layoutParent, Tree* expression,
                        int parentPriority);
  bool implicitAddition(const Tree* addition);
  void serializeDecimalOrFloat(const Tree* expression, char* buffer,
                               size_t bufferSize);
  // Recursively replace "+-" into "-" in rack
  static void StripUselessPlus(Tree* rack);

  bool linearMode() const {
    return m_parameters.layouterMode != LayouterMode::Natural;
  }

  LayouterParameters m_parameters;
  bool m_addSeparators;
};
}  // namespace Poincare::Internal
