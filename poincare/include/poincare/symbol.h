#ifndef POINCARE_SYMBOL_H
#define POINCARE_SYMBOL_H

#include <poincare/symbol_abstract.h>

namespace Poincare {

class SymbolNode final : public SymbolAbstractNode {
 public:
  SymbolNode(const char* newName, int length);

  const char* name() const override { return m_name; }

  // TreeNode
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "Symbol"; }
#endif

  // Expression Properties
  Type type() const override { return Type::Symbol; }
  // Symbol is not expanded for sign as it may be a local variable.
  TrinaryBoolean isPositive(Context* context) const override {
    return TrinaryBoolean::Unknown;
  }
  int polynomialDegree(Context* context, const char* symbolName) const override;
  int getPolynomialCoefficients(Context* context, const char* symbolName,
                                Expression coefficients[]) const override;
  int getVariables(Context* context, isVariableTest isVariable, char* variables,
                   int maxSizeVariable, int nextVariableIndex) const override;
  double degreeForSortingAddition(bool symbolsOnly) const override {
    return 1.;
  }
  /* getUnit returns Undefined, because the symbol would have
   * already been replaced if it should have been.*/

  /* Layout */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                      int numberOfSignificantDigits,
                      Context* context) const override;

  /* Simplification */
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  Expression deepReplaceReplaceableSymbols(
      Context* context, TrinaryBoolean* isCircular,
      int parameteredAncestorsCount,
      SymbolicComputation symbolicComputation) override;
  LayoutShape leftLayoutShape() const override;

  /* Derivation */
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                Expression symbolValue) override;

  /* Approximation */
  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<float>(approximationContext);
  }
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<double>(approximationContext);
  }

  bool isUnknown() const;

 private:
  char m_name[0];  // MUST be the last member variable

  size_t nodeSize() const override { return sizeof(SymbolNode); }
  template <typename T>
  Evaluation<T> templatedApproximate(
      const ApproximationContext& approximationContext) const;

  // Name
  char* editableName() override { return m_name; }
};

class Symbol final : public SymbolAbstract {
  friend class Expression;
  friend class SymbolNode;

 public:
  constexpr static AliasesList k_ansAliases = AliasesLists::k_ansAliases;
  static Symbol SystemSymbol() { return Builder(UCodePointUnknown); }

  Symbol(const SymbolNode* node = nullptr) : SymbolAbstract(node) {}
  static Symbol Builder(const char* name, int length) {
    return SymbolAbstract::Builder<Symbol, SymbolNode>(name, length);
  }
  static Symbol Builder(CodePoint name);
  static Symbol Ans() {
    return Symbol::Builder(k_ansAliases.mainAlias(),
                           strlen(k_ansAliases.mainAlias()));
  }

  // Symbol properties
  bool isSystemSymbol() const { return node()->isUnknown(); }
  const char* name() const { return node()->name(); }

  // Expression
  Expression shallowReduce(ReductionContext reductionContext);
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                Expression symbolValue);
  int getPolynomialCoefficients(Context* context, const char* symbolName,
                                Expression coefficients[]) const;
  Expression deepReplaceReplaceableSymbols(
      Context* context, TrinaryBoolean* isCircular,
      int parameteredAncestorsCount, SymbolicComputation symbolicComputation);

 private:
  SymbolNode* node() const {
    return static_cast<SymbolNode*>(Expression::node());
  }
};

}  // namespace Poincare

#endif
