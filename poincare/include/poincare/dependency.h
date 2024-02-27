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
  void logNodeName(std::ostream& stream) const override {
    stream << "Dependency";
  }
#endif

  // Properties
  Type type() const override { return Type::Dependency; }
  int polynomialDegree(Context* context,
                       const char* symbolName) const override {
    return mainExpression()->polynomialDegree(context, symbolName);
  }
  int getPolynomialCoefficients(Context* context, const char* symbolName,
                                Expression coefficients[]) const override;

  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  OLayout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                       int numberOfSignificantDigits,
                       Context* context) const override {
    assert(false);
    return mainExpression()->createLayout(floatDisplayMode,
                                          numberOfSignificantDigits, context);
  }
  LayoutShape leftLayoutShape() const override {
    return mainExpression()->leftLayoutShape();
  }

  // Evaluation
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

  // Simplification
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                Expression symbolValue) override;

 private:
  template <typename T>
  Evaluation<T> templatedApproximate(
      const ApproximationContext& approximationContext) const;

  ExpressionNode* mainExpression() const;
  ExpressionNode* dependenciesList() const;
};

class Dependency : public Expression {
  friend class DependencyNode;

 public:
  constexpr static int k_indexOfMainExpression = 0;
  constexpr static int k_indexOfDependenciesList = 1;
  Dependency(const DependencyNode* n) : Expression(n) {}
  static Dependency Builder(Expression expression,
                            List dependencies = List::Builder()) {
    return TreeHandle::FixedArityBuilder<Dependency, DependencyNode>(
        {expression, dependencies});
  }

  void deepReduceChildren(const ReductionContext& reductionContext);
  Expression shallowReduce(ReductionContext reductionContext);

  int numberOfDependencies() const {
    return dependenciesList().numberOfChildren();
  }
  void addDependency(Expression newDependency);

  /* Store the dependecies in l and replace the dependency node with the true
   * expression. */
  Expression extractDependencies(List l);
  bool dependencyRecursivelyMatches(
      ExpressionTrinaryTest test, Context* context,
      SymbolicComputation replaceSymbols, void* auxiliary,
      Expression::IgnoredSymbols* ignoredSymbols) const {
    return mainExpression().recursivelyMatches(test, context, replaceSymbols,
                                               auxiliary, ignoredSymbols);
  }

  Expression removeUselessDependencies(
      const ReductionContext& reductionContext);

  // Parser utils
  static Expression UntypedBuilder(Expression children);
  static_assert(UCodePointSystem == 0x14,
                "UCodePointSystem value was modified");
  constexpr static Expression::FunctionHelper s_functionHelper =
      Expression::FunctionHelper("\u0014dep", 2, &UntypedBuilder);

 private:
  Expression mainExpression() const {
    return childAtIndex(k_indexOfMainExpression);
  }
  Expression dependenciesList() const {
    return childAtIndex(k_indexOfDependenciesList);
  }
};

}  // namespace Poincare

#endif
