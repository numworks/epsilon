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

  class Or final : public Expression {
  public:
    Or(const BinaryOperationNode<5> *n) : Expression(n) {}
    static Or Builder(Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<Or, BinaryOperationNode<5> >({child1, child2}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("or", 2, &UntypedBuilderTwoChildren<Or>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class Xor final : public Expression {
  public:
    Xor(const BinaryOperationNode<9> *n) : Expression(n) {}
    static Xor Builder(Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<Xor, BinaryOperationNode<9> >({child1, child2}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("xor", 2, &UntypedBuilderTwoChildren<Xor>);
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

  class BitClear final : public Expression {
  public:
    BitClear(const BinaryOperationNode<15> *n) : Expression(n) {}
    static BitClear Builder(Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<BitClear, BinaryOperationNode<15> >({child1, child2}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("bclr", 2, &UntypedBuilderTwoChildren<BitClear>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class BitFlip final : public Expression {
  public:
    BitFlip(const BinaryOperationNode<16> *n) : Expression(n) {}
    static BitFlip Builder(Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<BitFlip, BinaryOperationNode<16> >({child1, child2}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("bflp", 2, &UntypedBuilderTwoChildren<BitFlip>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class BitGet final : public Expression {
  public:
    BitGet(const BinaryOperationNode<17> *n) : Expression(n) {}
    static BitGet Builder(Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<BitGet, BinaryOperationNode<17> >({child1, child2}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("bit", 2, &UntypedBuilderTwoChildren<BitGet>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };
  class BitSet final : public Expression {
  public:
    BitSet(const BinaryOperationNode<18> *n) : Expression(n) {}
    static BitSet Builder(Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<BitSet, BinaryOperationNode<18> >({child1, child2}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("bset", 2, &UntypedBuilderTwoChildren<BitSet>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class BitsClear final : public Expression {
  public:
    BitsClear(const BinaryOperationNode<19> *n) : Expression(n) {}
    static BitsClear Builder(Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<BitsClear, BinaryOperationNode<19> >({child1, child2}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("bic", 2, &UntypedBuilderTwoChildren<BitsClear>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class BitsClearExplicit final : public Expression {
  public:
    BitsClearExplicit(const BinaryOperationNode<20> *n) : Expression(n) {}
    static BitsClearExplicit Builder(Expression child1, Expression child2, Expression child3) { return TreeHandle::FixedArityBuilder<BitsClearExplicit, BinaryOperationNode<20> >({child1, child2, child3}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("bic", 3, &UntypedBuilderThreeChildren<BitsClearExplicit>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class ShiftLogicLeft final : public Expression {
  public:
    ShiftLogicLeft(const BinaryOperationNode<21> *n) : Expression(n) {}
    static ShiftLogicLeft Builder(Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<ShiftLogicLeft, BinaryOperationNode<21> >({child1, child2}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("sll", 2, &UntypedBuilderTwoChildren<ShiftLogicLeft>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class ShiftLogicLeftExplicit final : public Expression {
  public:
    ShiftLogicLeftExplicit(const BinaryOperationNode<22> *n) : Expression(n) {}
    static ShiftLogicLeftExplicit Builder(Expression child1, Expression child2, Expression child3) { return TreeHandle::FixedArityBuilder<ShiftLogicLeftExplicit, BinaryOperationNode<22> >({child1, child2, child3}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("sll", 3, &UntypedBuilderThreeChildren<ShiftLogicLeftExplicit>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class ShiftLogicRight final : public Expression {
  public:
    ShiftLogicRight(const BinaryOperationNode<23> *n) : Expression(n) {}
    static ShiftLogicRight Builder(Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<ShiftLogicRight, BinaryOperationNode<23> >({child1, child2}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("srl", 2, &UntypedBuilderTwoChildren<ShiftLogicRight>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class ShiftLogicRightExplicit final : public Expression {
  public:
    ShiftLogicRightExplicit(const BinaryOperationNode<24> *n) : Expression(n) {}
    static ShiftLogicRightExplicit Builder(Expression child1, Expression child2, Expression child3) { return TreeHandle::FixedArityBuilder<ShiftLogicRightExplicit, BinaryOperationNode<24> >({child1, child2, child3}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("srl", 3, &UntypedBuilderThreeChildren<ShiftLogicRightExplicit>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class ShiftArithmeticRight final : public Expression {
  public:
    ShiftArithmeticRight(const BinaryOperationNode<25> *n) : Expression(n) {}
    static ShiftArithmeticRight Builder(Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<ShiftArithmeticRight, BinaryOperationNode<25> >({child1, child2}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("sra", 2, &UntypedBuilderTwoChildren<ShiftArithmeticRight>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class ShiftArithmeticRightExplicit final : public Expression {
  public:
    ShiftArithmeticRightExplicit(const BinaryOperationNode<26> *n) : Expression(n) {}
    static ShiftArithmeticRightExplicit Builder(Expression child1, Expression child2, Expression child3) { return TreeHandle::FixedArityBuilder<ShiftArithmeticRightExplicit, BinaryOperationNode<26> >({child1, child2, child3}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("sra", 3, &UntypedBuilderThreeChildren<ShiftArithmeticRightExplicit>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class RotateLeft final : public Expression {
  public:
    RotateLeft(const BinaryOperationNode<27> *n) : Expression(n) {}
    static RotateLeft Builder(Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<RotateLeft, BinaryOperationNode<27> >({child1, child2}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("rol", 2, &UntypedBuilderTwoChildren<RotateLeft>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class RotateLeftExplicit final : public Expression {
  public:
    RotateLeftExplicit(const BinaryOperationNode<28> *n) : Expression(n) {}
    static RotateLeftExplicit Builder(Expression child1, Expression child2, Expression child3) { return TreeHandle::FixedArityBuilder<RotateLeftExplicit, BinaryOperationNode<28> >({child1, child2, child3}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("rol", 3, &UntypedBuilderThreeChildren<RotateLeftExplicit>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class RotateRight final : public Expression {
  public:
    RotateRight(const BinaryOperationNode<29> *n) : Expression(n) {}
    static RotateRight Builder(Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<RotateRight, BinaryOperationNode<29> >({child1, child2}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("ror", 2, &UntypedBuilderTwoChildren<RotateRight>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class RotateRightExplicit final : public Expression {
  public:
    RotateRightExplicit(const BinaryOperationNode<30> *n) : Expression(n) {}
    static RotateRightExplicit Builder(Expression child1, Expression child2, Expression child3) { return TreeHandle::FixedArityBuilder<RotateRightExplicit, BinaryOperationNode<30> >({child1, child2, child3}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("ror", 3, &UntypedBuilderThreeChildren<RotateRightExplicit>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

  class TwosComplement final : public Expression {
  public:
    TwosComplement(const BinaryOperationNode<31> *n) : Expression(n) {}
    static TwosComplement Builder(Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<TwosComplement, BinaryOperationNode<31> >({child1, child2}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("tc", 2, &UntypedBuilderTwoChildren<TwosComplement>);
    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

} // namespace Poincare

#endif