#include <poincare/expression.h>
#include "../expression_debug.h"
#include <iostream>
#include <poincare/static_hierarchy.h>
#include "demo_ruleset.h"

namespace Poincare {

class SimplificationRoot : public StaticHierarchy<1> {
public:
  SimplificationRoot(Expression * e) : StaticHierarchy<1>(&e, false) {
    e->setParent(this);
  }
  ~SimplificationRoot() {
    detachOperand(operand(0));
    /* We don't want to clone the expression provided at construction.
     * So we don't want it to be deleted when we're destroyed (parent destructor). */
  }
  bool isCommutative() const override { return false; }
  Expression * clone() const override { return nullptr; }
  Type type() const override { return Expression::Type::Undefined; }
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override {
    return nullptr;
  }
  Evaluation<float> * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override {
    return nullptr;
  }
  Evaluation<double> * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override {
    return nullptr;
  }
};

void Expression::simplify(Expression ** e) {
  SimplificationRoot root(*e);
  root.sort();
  // Only unconditional simplify
  int ruleIndex = 0;
  while (ruleIndex < Simplification::DemoRuleset.numberOfRules()) {
    const Simplification::Rule * rule = Simplification::DemoRuleset.ruleAtIndex(ruleIndex++);
    if (rule->apply(&root)) {
      root.sort();
      std::cout << "-----" << std::endl;
      print_expression(root.operand(0), 0);
      std::cout << "-----" << std::endl;
      ruleIndex = 0;
    }
  }
  *e = (Expression *)(root.operand(0));
}

}
