#ifndef POINCARE_DEPENDENCY_H
#define POINCARE_DEPENDENCY_H

#include <poincare/list.h>

namespace Poincare {

class DependencyNode : public ExpressionNode {
public:
  // TreeNode
  size_t size() const override { return sizeof(DependencyNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Dependency";
  }
#endif

  // Properties
  Type type() const override { return Type::Dependency; }
  int polynomialDegree(Context * context, const char * symbolName) const override { return childAtIndex(0)->polynomialDegree(context, symbolName); }
  int getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[]) const override { return childAtIndex(0)->getPolynomialCoefficients(context, symbolName, coefficients); }

  // Layout
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override { assert(false); return Layout(); }
  LayoutShape leftLayoutShape() const override { return childAtIndex(0)->leftLayoutShape(); }

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<double>(approximationContext); }

  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  bool derivate(ReductionContext reductionContext, Symbol symbol, Expression symbolValue) override;

private:
  template<typename T> Evaluation<T> templatedApproximate(ApproximationContext approximationContext) const;
};

class Dependency : public Expression {
  friend class DependencyNode;
public:
  Dependency(const DependencyNode * n) : Expression(n) {}
  static Dependency Builder(Expression expression, List dependencies = List::Builder()) { return TreeHandle::FixedArityBuilder<Dependency, DependencyNode>({expression, dependencies}); }

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);

  int numberOfDependencies() const { return childAtIndex(1).numberOfChildren(); }
  void addDependency(Expression newDependency);
  /* Store the dependecies in m and replace the dependency node with the true expression. */
  void extractDependencies(List l);
  bool dependencyRecursivelyMatches(ExpressionTernaryTest test, Context * context, ExpressionNode::SymbolicComputation replaceSymbols = ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition, void * auxiliary = nullptr) const { return childAtIndex(0).recursivelyMatches(test, context, replaceSymbols, auxiliary); }

  // Parser utils
  static Expression UntypedBuilder(Expression children);
  static_assert(UCodePointSystem == 0x14, "UCodePointSystem value was modified");
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("dep\x14", 2, &UntypedBuilder);
};

}

#endif
