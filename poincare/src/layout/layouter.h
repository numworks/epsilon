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
  LayouterMode layouterMode = LayouterMode::Natural;
  int numberOfSignificantDigits =
      PrintFloat::k_undefinedNumberOfSignificantDigits;
  Preferences::PrintFloatMode floatMode = Preferences::PrintFloatMode::Decimal;
  OMG::Base base = OMG::Base::Decimal;
#if POINCARE_NO_E_EXPONENT
  constexpr static bool useTenPowerInsteadOfE = true;
#else
  bool useTenPowerInsteadOfE = false;
#endif
};

class Layouter {
 public:
  // Create a Layout corresponding to the given expression
  static Tree* LayoutExpression(const Tree* expression,
                                LayouterParameters params = {});

  static bool AddThousandsSeparators(Tree* rack);

  /* Recursively remove OperatorSeparators, UnitSeparators and
   * ThousandsSeparators in rack */
  static void StripSeparators(Tree* rack);

 private:
  Layouter(LayouterParameters params)
      : m_parameters(params), m_addSeparators(false) {}

  // May raise TreeStack exceptions
  static Tree* UnsafeLayoutExpression(const Tree* expression,
                                      LayouterParameters params = {});

  // Insert layout or codepoint depending on linear mode.
  Tree* insertParenthesis(Tree* parentRack, bool isOpening,
                          bool isCurlyBrace = false);
  /* TODO: addOperatorSeparator, addUnitSeparator and AddThousandsSeparators
   * should all be applicable in linear mode. Separators should be stripped
   * before parsing layouts in 1D. */
  void addOperatorSeparator(Tree* parentRack);
  void addUnitSeparator(Tree* parentRack);
  bool requireSeparators(const Tree* expression);
  void layoutText(Tree* layoutParent, std::string_view text);
  void layoutBuiltin(Tree* layoutParent, const Tree* expression);
  void layoutFunctionCall(Tree* layoutParent, const Tree* expression,
                          std::string_view name);
  void layoutChildrenAsRacks(const Tree* expression);
  void layoutIntegerHandler(Tree* parentRack, IntegerHandler handler,
                            int decimalOffset = 0);
  void layoutInfixOperator(Tree* parentRack, const Tree* expression,
                           CodePoint op, bool multiplication = false);
  void layoutMatrix(Tree* parentRack, const Tree* expression);
  void layoutSequence(Tree* parentRack, const Tree* expression);
  void layoutUnit(Tree* parentRack, const Tree* expression);
  void layoutPowerOrDivision(Tree* parentRack, const Tree* expression);
  void layoutExpression(Tree* parentRack, const Tree* expression,
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
