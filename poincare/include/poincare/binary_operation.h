#ifndef POINCARE_BINARY_OPERATION_H
#define POINCARE_BINARY_OPERATION_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare
{

  template<int T>
  class BinaryOperationNode final : public ExpressionNode {
  public:
    // TreeNode
    size_t size() const override { return sizeof(BinaryOperationNode); }
    int numberOfChildren() const override;
#if POINCARE_TREE_LOG
    void logNodeName(std::ostream &stream) const override
    {
      stream << "BinaryOperation";
    }
#endif

    // Properties
    Type type() const override { return Type::And; }

    // Layout
    Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
    int serialize(char *buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
    // Simplification
    Expression shallowReduce(ReductionContext reductionContext) override;
    LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
    LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }

    // Evaluation
    Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<float>(approximationContext); }
    Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<double>(approximationContext); }
    
    template <typename U>
    Evaluation<U> templatedApproximate(ApproximationContext approximationContext) const {
      return Complex<U>::RealUndefined();
    }
  };

  class And final : public Expression {
  public:
    And(const BinaryOperationNode<1> *n) : Expression(n) {}
    static And Builder(Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<And, BinaryOperationNode<1> >({child1, child2}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("and", 2, &UntypedBuilderTwoChildren<And>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class AndExplicit final : public Expression {
  public:
    AndExplicit(const BinaryOperationNode<2> *n) : Expression(n) {}
    static AndExplicit Builder(Expression child1, Expression child2, Expression child3) { return TreeHandle::FixedArityBuilder<AndExplicit, BinaryOperationNode<2> >({child1, child2, child3}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("and", 3, &UntypedBuilderThreeChildren<AndExplicit>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class Nand final : public Expression {
  public:
    Nand(const BinaryOperationNode<3> *n) : Expression(n) {}
    static Nand Builder(Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<Nand, BinaryOperationNode<3> >({child1, child2}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("nand", 2, &UntypedBuilderTwoChildren<Nand>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class NandExplicit final : public Expression {
  public:
    NandExplicit(const BinaryOperationNode<4> *n) : Expression(n) {}
    static NandExplicit Builder(Expression child1, Expression child2, Expression child3) { return TreeHandle::FixedArityBuilder<NandExplicit, BinaryOperationNode<4> >({child1, child2, child3}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("nand", 3, &UntypedBuilderThreeChildren<NandExplicit>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class Or final : public Expression {
  public:
    Or(const BinaryOperationNode<5> *n) : Expression(n) {}
    static Or Builder(Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<Or, BinaryOperationNode<5> >({child1, child2}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("or", 2, &UntypedBuilderTwoChildren<Or>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class OrExplicit final : public Expression {
  public:
    OrExplicit(const BinaryOperationNode<6> *n) : Expression(n) {}
    static OrExplicit Builder(Expression child1, Expression child2, Expression child3) { return TreeHandle::FixedArityBuilder<OrExplicit, BinaryOperationNode<6> >({child1, child2, child3}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("or", 3, &UntypedBuilderThreeChildren<OrExplicit>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class Nor final : public Expression {
  public:
    Nor(const BinaryOperationNode<7> *n) : Expression(n) {}
    static Nor Builder(Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<Nor, BinaryOperationNode<7> >({child1, child2}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("nor", 2, &UntypedBuilderTwoChildren<Nor>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class NorExplicit final : public Expression {
  public:
    NorExplicit(const BinaryOperationNode<8> *n) : Expression(n) {}
    static NorExplicit Builder(Expression child1, Expression child2, Expression child3) { return TreeHandle::FixedArityBuilder<NorExplicit, BinaryOperationNode<8> >({child1, child2, child3}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("nor", 3, &UntypedBuilderThreeChildren<NorExplicit>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class Xor final : public Expression {
  public:
    Xor(const BinaryOperationNode<9> *n) : Expression(n) {}
    static Xor Builder(Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<Xor, BinaryOperationNode<9> >({child1, child2}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("xor", 2, &UntypedBuilderTwoChildren<Xor>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class XorExplicit final : public Expression {
  public:
    XorExplicit(const BinaryOperationNode<10> *n) : Expression(n) {}
    static XorExplicit Builder(Expression child1, Expression child2, Expression child3) { return TreeHandle::FixedArityBuilder<XorExplicit, BinaryOperationNode<10> >({child1, child2, child3}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("xor", 3, &UntypedBuilderThreeChildren<XorExplicit>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class Xnor final : public Expression {
  public:
    Xnor(const BinaryOperationNode<11> *n) : Expression(n) {}
    static Xnor Builder(Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<Xnor, BinaryOperationNode<11> >({child1, child2}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("xnor", 2, &UntypedBuilderTwoChildren<Xnor>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class XnorExplicit final : public Expression {
  public:
    XnorExplicit(const BinaryOperationNode<12> *n) : Expression(n) {}
    static XnorExplicit Builder(Expression child1, Expression child2, Expression child3) { return TreeHandle::FixedArityBuilder<XnorExplicit, BinaryOperationNode<12> >({child1, child2, child3}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("xnor", 3, &UntypedBuilderThreeChildren<XnorExplicit>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class Not final : public Expression {
  public:
    Not(const BinaryOperationNode<13> *n) : Expression(n) {}
    static Not Builder(Expression child1) { return TreeHandle::FixedArityBuilder<Not, BinaryOperationNode<13> >({child1}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("not", 1, &UntypedBuilderOneChild<Not>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class NotExplicit final : public Expression {
  public:
    NotExplicit(const BinaryOperationNode<14> *n) : Expression(n) {}
    static NotExplicit Builder(Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<NotExplicit, BinaryOperationNode<14> >({child1, child2}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("not", 2, &UntypedBuilderTwoChildren<NotExplicit>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

} // namespace Poincare

#endif
