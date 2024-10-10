#ifndef POINCARE_DEPENDENCY_H
#define POINCARE_DEPENDENCY_H

#include "list.h"

namespace Poincare {

class DependencyNode : public ExpressionNode {
 public:
  // PoolObject
  size_t size() const override { return sizeof(DependencyNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Dependency";
  }
#endif

  // Properties
  Type otype() const override { return Type::Dependency; }
  int getPolynomialCoefficients(Context* context, const char* symbolName,
                                OExpression coefficients[]) const override;

  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  LayoutShape leftLayoutShape() const override {
    return mainExpression()->leftLayoutShape();
  }

  // Simplification
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue) override;

 private:
  ExpressionNode* mainExpression() const;
  ExpressionNode* dependenciesList() const;
};

class Dependency : public OExpression {
  friend class DependencyNode;

 public:
  constexpr static int k_indexOfMainExpression = 0;
  constexpr static int k_indexOfDependenciesList = 1;
  Dependency(const DependencyNode* n) : OExpression(n) {}
  static Dependency Builder(OExpression expression,
                            OList dependencies = OList::Builder()) {
    return PoolHandle::FixedArityBuilder<Dependency, DependencyNode>(
        {expression, dependencies});
  }

  void deepReduceChildren(const ReductionContext& reductionContext);
  OExpression shallowReduce(ReductionContext reductionContext);

  int numberOfDependencies() const {
    return dependenciesList().numberOfChildren();
  }
  void addDependency(OExpression newDependency);

  /* Store the dependecies in l and replace the dependency node with the true
   * expression. */
  OExpression extractDependencies(OList l);

  OExpression removeUselessDependencies(
      const ReductionContext& reductionContext);

  // Parser utils
  static OExpression UntypedBuilder(OExpression children);
  static_assert(UCodePointSystem == 0x14,
                "UCodePointSystem value was modified");
  constexpr static OExpression::FunctionHelper s_functionHelper =
      OExpression::FunctionHelper("\u0014dep", 2, &UntypedBuilder);

 private:
  OExpression mainExpression() const {
    return childAtIndex(k_indexOfMainExpression);
  }
  OExpression dependenciesList() const {
    return childAtIndex(k_indexOfDependenciesList);
  }
};

}  // namespace Poincare

#endif
