#ifndef POINCARE_SYMBOL_H
#define POINCARE_SYMBOL_H

#include "symbol_abstract.h"

namespace Poincare {

class SymbolNode final : public SymbolAbstractNode {
 public:
  using SymbolAbstractNode::SymbolAbstractNode;

  // PoolObject
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "Symbol"; }
#endif

  // OExpression Properties
  Type otype() const override { return Type::Symbol; }
  // Symbol is not expanded for sign as it may be a local variable.
  OMG::Troolean isPositive(Context* context) const override {
    return OMG::Troolean::Unknown;
  }
  int getPolynomialCoefficients(Context* context, const char* symbolName,
                                OExpression coefficients[]) const override;
  int getVariables(Context* context, isVariableTest isVariable, char* variables,
                   int maxSizeVariable, int nextVariableIndex) const override;
  double degreeForSortingAddition(bool symbolsOnly) const override {
    return 1.;
  }
  /* getUnit returns Undefined, because the symbol would have
   * already been replaced if it should have been.*/

  /* Layout */

  /* Simplification */
  OExpression shallowReduce(const ReductionContext& reductionContext) override;

  /* Derivation */
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue) override;

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

  bool isSystemSymbol() const;

 private:
  size_t nodeSize() const override { return sizeof(SymbolNode); }
  template <typename T>
  Evaluation<T> templatedApproximate(
      const ApproximationContext& approximationContext) const;
};

class Symbol final : public SymbolAbstract {
  friend class OExpression;
  friend class SymbolNode;

 public:
  constexpr static CodePoint k_cartesianSymbol = 'x';
  constexpr static CodePoint k_parametricSymbol = 't';
  constexpr static CodePoint k_polarSymbol = UCodePointGreekSmallLetterTheta;
  constexpr static CodePoint k_radiusSymbol = 'r';
  constexpr static CodePoint k_ordinateSymbol = 'y';
  constexpr static CodePoint k_sequenceSymbol = 'n';
  constexpr static AliasesList k_ansAliases = AliasesLists::k_ansAliases;

  Symbol(const SymbolNode* node = nullptr) : SymbolAbstract(node) {}

  // Symbol properties
  bool isSystemSymbol() const { return node()->isSystemSymbol(); }

  // OExpression
  OExpression shallowReduce(ReductionContext reductionContext);
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue);
  int getPolynomialCoefficients(Context* context, const char* symbolName,
                                OExpression coefficients[]) const;

 private:
  SymbolNode* node() const {
    return static_cast<SymbolNode*>(OExpression::node());
  }
};

}  // namespace Poincare

#endif
