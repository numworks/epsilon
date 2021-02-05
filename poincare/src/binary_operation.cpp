#include <poincare/binary_operation.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/undefined.h>
#include <poincare/rational.h>
#include <poincare/based_integer.h>
#include <cmath>

namespace Poincare {

  constexpr Expression::FunctionHelper And::s_functionHelper;
  constexpr Expression::FunctionHelper AndExplicit::s_functionHelper;
  constexpr Expression::FunctionHelper Nand::s_functionHelper;
  constexpr Expression::FunctionHelper NandExplicit::s_functionHelper;
  constexpr Expression::FunctionHelper Or::s_functionHelper;
  constexpr Expression::FunctionHelper OrExplicit::s_functionHelper;
  constexpr Expression::FunctionHelper Nor::s_functionHelper;
  constexpr Expression::FunctionHelper NorExplicit::s_functionHelper;
  constexpr Expression::FunctionHelper Xor::s_functionHelper;
  constexpr Expression::FunctionHelper XorExplicit::s_functionHelper;
  constexpr Expression::FunctionHelper Xnor::s_functionHelper;
  constexpr Expression::FunctionHelper XnorExplicit::s_functionHelper;
  constexpr Expression::FunctionHelper Not::s_functionHelper;
  constexpr Expression::FunctionHelper NotExplicit::s_functionHelper;
  constexpr Expression::FunctionHelper BitClear::s_functionHelper;
  constexpr Expression::FunctionHelper BitFlip::s_functionHelper;
  constexpr Expression::FunctionHelper BitGet::s_functionHelper;
  constexpr Expression::FunctionHelper BitSet::s_functionHelper;
  constexpr Expression::FunctionHelper BitsClear::s_functionHelper;
  constexpr Expression::FunctionHelper BitsClearExplicit::s_functionHelper;
  constexpr Expression::FunctionHelper ShiftLogicLeft::s_functionHelper;
  constexpr Expression::FunctionHelper ShiftLogicLeftExplicit::s_functionHelper;
  constexpr Expression::FunctionHelper ShiftLogicRight::s_functionHelper;
  constexpr Expression::FunctionHelper ShiftLogicRightExplicit::s_functionHelper;
  constexpr Expression::FunctionHelper ShiftArithmeticRight::s_functionHelper;
  constexpr Expression::FunctionHelper ShiftArithmeticRightExplicit::s_functionHelper;
  constexpr Expression::FunctionHelper RotateLeft::s_functionHelper;
  constexpr Expression::FunctionHelper RotateLeftExplicit::s_functionHelper;
  constexpr Expression::FunctionHelper RotateRight::s_functionHelper;
  constexpr Expression::FunctionHelper RotateRightExplicit::s_functionHelper;
  constexpr Expression::FunctionHelper TwosComplement::s_functionHelper;

  template<> int BinaryOperationNode<1>::numberOfChildren() const { return And::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<2>::numberOfChildren() const { return AndExplicit::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<3>::numberOfChildren() const { return Nand::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<4>::numberOfChildren() const { return NandExplicit::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<5>::numberOfChildren() const { return Or::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<6>::numberOfChildren() const { return OrExplicit::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<7>::numberOfChildren() const { return Nor::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<8>::numberOfChildren() const { return NorExplicit::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<9>::numberOfChildren() const { return Xor::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<10>::numberOfChildren() const { return XorExplicit::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<11>::numberOfChildren() const { return Xnor::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<12>::numberOfChildren() const { return XnorExplicit::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<13>::numberOfChildren() const { return Not::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<14>::numberOfChildren() const { return NotExplicit::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<15>::numberOfChildren() const { return BitClear::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<16>::numberOfChildren() const { return BitFlip::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<17>::numberOfChildren() const { return BitGet::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<18>::numberOfChildren() const { return BitSet::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<19>::numberOfChildren() const { return BitsClear::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<20>::numberOfChildren() const { return BitsClearExplicit::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<21>::numberOfChildren() const { return ShiftLogicLeft::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<22>::numberOfChildren() const { return ShiftLogicLeftExplicit::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<23>::numberOfChildren() const { return ShiftLogicRight::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<24>::numberOfChildren() const { return ShiftLogicRightExplicit::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<25>::numberOfChildren() const { return ShiftArithmeticRight::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<26>::numberOfChildren() const { return ShiftArithmeticRightExplicit::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<27>::numberOfChildren() const { return RotateLeft::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<28>::numberOfChildren() const { return RotateLeftExplicit::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<29>::numberOfChildren() const { return RotateRight::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<30>::numberOfChildren() const { return RotateRightExplicit::s_functionHelper.numberOfChildren(); }
  template<> int BinaryOperationNode<31>::numberOfChildren() const { return TwosComplement::s_functionHelper.numberOfChildren(); }

  template<>
  Layout BinaryOperationNode<1>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, And::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<2>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, AndExplicit::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<3>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, Nand::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<4>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, NandExplicit::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<5>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, Or::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<6>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, OrExplicit::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<7>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, Nor::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<8>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, NorExplicit::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<9>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, Xor::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<10>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, XorExplicit::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<11>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, Xnor::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<12>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, XnorExplicit::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<13>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, Not::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<14>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, NotExplicit::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<15>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, BitClear::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<16>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, BitFlip::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<17>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, BitGet::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<18>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, BitSet::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<19>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, BitsClear::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<20>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, BitsClearExplicit::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<21>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, ShiftLogicLeft::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<22>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, ShiftLogicLeftExplicit::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<23>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, ShiftLogicRight::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<24>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, ShiftLogicRightExplicit::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<25>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, ShiftArithmeticRight::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<26>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, ShiftArithmeticRightExplicit::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<27>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, RotateLeft::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<28>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, RotateLeftExplicit::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<29>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, RotateRight::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<30>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, RotateRightExplicit::s_functionHelper.name());
  }

  template<>
  Layout BinaryOperationNode<31>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, TwosComplement::s_functionHelper.name());
  }


  template<int T>
  int BinaryOperationNode<T>::serialize(char *buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      T == 31 ? TwosComplement::s_functionHelper.name() : 
      T == 30 ? RotateRightExplicit::s_functionHelper.name() : 
      T == 29 ? RotateRight::s_functionHelper.name() : 
      T == 28 ? RotateLeftExplicit::s_functionHelper.name() : 
      T == 27 ? RotateLeft::s_functionHelper.name() : 
      T == 26 ? ShiftArithmeticRightExplicit::s_functionHelper.name() : 
      T == 25 ? ShiftArithmeticRight::s_functionHelper.name() : 
      T == 24 ? ShiftLogicRightExplicit::s_functionHelper.name() : 
      T == 23 ? ShiftLogicRight::s_functionHelper.name() : 
      T == 22 ? ShiftLogicLeftExplicit::s_functionHelper.name() : 
      T == 21 ? ShiftLogicLeft::s_functionHelper.name() : 
      T == 20 ? BitsClearExplicit::s_functionHelper.name() : 
      T == 19 ? BitsClear::s_functionHelper.name() : 
      T == 18 ? BitSet::s_functionHelper.name() : 
      T == 17 ? BitGet::s_functionHelper.name() : 
      T == 16 ? BitFlip::s_functionHelper.name() : 
      T == 15 ? BitClear::s_functionHelper.name() : 
      T == 14 ? NotExplicit::s_functionHelper.name() : 
      T == 13 ? Not::s_functionHelper.name() : 
      T == 12 ? XnorExplicit::s_functionHelper.name() : 
      T == 11 ? Xnor::s_functionHelper.name() : 
      T == 10 ? XorExplicit::s_functionHelper.name() : 
      T == 9 ? Xor::s_functionHelper.name() : 
      T == 8 ? NorExplicit::s_functionHelper.name() : 
      T == 7 ? Nor::s_functionHelper.name() : 
      T == 6 ? OrExplicit::s_functionHelper.name() : 
      T == 5 ? Or::s_functionHelper.name() : 
      T == 4 ? NandExplicit::s_functionHelper.name() : 
      T == 3 ? Nand::s_functionHelper.name() : 
      T == 2 ? AndExplicit::s_functionHelper.name() : 
      And::s_functionHelper.name()
    );
  }

  template<>
  Expression BinaryOperationNode<1>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return And(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<2>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return AndExplicit(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<3>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return Nand(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<4>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return NandExplicit(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<5>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return Or(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<6>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return OrExplicit(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<7>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return Nor(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<8>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return NorExplicit(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<9>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return Xor(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<10>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return XorExplicit(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<11>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return Xnor(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<12>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return XnorExplicit(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<13>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return Not(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<14>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return NotExplicit(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<15>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return BitClear(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<16>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return BitFlip(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<17>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return BitGet(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<18>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return BitSet(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<19>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return BitsClear(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<20>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return BitsClearExplicit(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<21>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return ShiftLogicLeft(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<22>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return ShiftLogicLeftExplicit(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<23>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return ShiftLogicRight(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<24>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return ShiftLogicRightExplicit(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<25>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return ShiftArithmeticRight(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<26>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return ShiftArithmeticRightExplicit(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<27>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return RotateLeft(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<28>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return RotateLeftExplicit(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<29>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return RotateRight(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<30>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return RotateRightExplicit(this).shallowReduce(reductionContext);
  }

  template<>
  Expression BinaryOperationNode<31>::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    return TwosComplement(this).shallowReduce(reductionContext);
  }

  // Check to make sure the the expression is a positive integer
  Integer getValidInteger(Expression a) {
    if (a.type() != ExpressionNode::Type::Rational) {
      return Integer(-1);
    }
    Rational ar = static_cast<Rational &>(a);
    if (!ar.integerDenominator().isOne()) {
      return Integer(-1);
    }
    if (ar.signedIntegerNumerator().isNegative()) {
      return Integer(-1);
    }
    return ar.signedIntegerNumerator();
  }

  Expression And::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));

    if(aq.isMinusOne() || bq.isMinusOne()) {
      return Undefined::Expression();
    }

    Integer x = Integer::LogicalAnd(aq, bq);
    Expression result = Rational::Builder(x);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression AndExplicit::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));
    Integer cq = getValidInteger(childAtIndex(2));

    if(aq.isMinusOne() || bq.isMinusOne() || cq.isMinusOne() || !cq.isLowerThan(Integer(__INT8_MAX__))) {
      return Undefined::Expression();
    }

    Integer x = Integer::LogicalAnd(aq, bq, cq);
    Expression result = Rational::Builder(x);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression Nand::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));

    if(aq.isMinusOne() || bq.isMinusOne()) {
      return Undefined::Expression();
    }

    Integer x = Integer::LogicalAnd(aq, bq);
    Integer nx = Integer::LogicalNot(x);
    Expression result = Rational::Builder(nx);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression NandExplicit::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));
    Integer cq = getValidInteger(childAtIndex(2));

    if(aq.isMinusOne() || bq.isMinusOne() || cq.isMinusOne() || !cq.isLowerThan(Integer(__INT8_MAX__))) {
      return Undefined::Expression();
    }

    Integer x = Integer::LogicalAnd(aq, bq, cq);
    Integer nx = Integer::LogicalNot(x, cq);
    Expression result = Rational::Builder(nx);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression Or::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));

    if(aq.isMinusOne() || bq.isMinusOne()) {
      return Undefined::Expression();
    }

    Integer x = Integer::LogicalOr(aq, bq);
    Expression result = Rational::Builder(x);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression OrExplicit::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));
    Integer cq = getValidInteger(childAtIndex(2));

    if(aq.isMinusOne() || bq.isMinusOne() || cq.isMinusOne() || !cq.isLowerThan(Integer(__INT8_MAX__))) {
      return Undefined::Expression();
    }

    Integer x = Integer::LogicalOr(aq, bq, cq);
    Expression result = Rational::Builder(x);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression Nor::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));

    if(aq.isMinusOne() || bq.isMinusOne()) {
      return Undefined::Expression();
    }

    Integer x = Integer::LogicalOr(aq, bq);
    Integer nx = Integer::LogicalNot(x);
    Expression result = Rational::Builder(nx);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression NorExplicit::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));
    Integer cq = getValidInteger(childAtIndex(2));

    if(aq.isMinusOne() || bq.isMinusOne() || cq.isMinusOne() || !cq.isLowerThan(Integer(__INT8_MAX__))) {
      return Undefined::Expression();
    }

    Integer x = Integer::LogicalOr(aq, bq, cq);
    Integer nx = Integer::LogicalNot(x, cq);
    Expression result = Rational::Builder(nx);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression Xor::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));

    if(aq.isMinusOne() || bq.isMinusOne()) {
      return Undefined::Expression();
    }

    Integer x = Integer::LogicalXor(aq, bq);
    Expression result = Rational::Builder(x);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression XorExplicit::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));
    Integer cq = getValidInteger(childAtIndex(2));

    if(aq.isMinusOne() || bq.isMinusOne() || cq.isMinusOne() || !cq.isLowerThan(Integer(__INT8_MAX__))) {
      return Undefined::Expression();
    }

    Integer x = Integer::LogicalXor(aq, bq, cq);
    Expression result = Rational::Builder(x);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression Xnor::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));

    if(aq.isMinusOne() || bq.isMinusOne()) {
      return Undefined::Expression();
    }

    Integer x = Integer::LogicalXor(aq, bq);
    Integer nx = Integer::LogicalNot(x);
    Expression result = Rational::Builder(nx);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression XnorExplicit::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));
    Integer cq = getValidInteger(childAtIndex(2));

    if(aq.isMinusOne() || bq.isMinusOne() || cq.isMinusOne() || !cq.isLowerThan(Integer(__INT8_MAX__))) {
      return Undefined::Expression();
    }

    Integer x = Integer::LogicalXor(aq, bq, cq);
    Integer nx = Integer::LogicalNot(x, cq);
    Expression result = Rational::Builder(nx);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression Not::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));

    if(aq.isMinusOne()) {
      return Undefined::Expression();
    }

    Integer n = Integer::LogicalNot(aq);
    Expression result = Rational::Builder(n);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression NotExplicit::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));

    if(aq.isMinusOne() || bq.isMinusOne() || !bq.isLowerThan(Integer(__INT8_MAX__))) {
      return Undefined::Expression();
    }

    Integer n = Integer::LogicalNot(aq, bq);
    Expression result = Rational::Builder(n);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression BitClear::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));

    if(aq.isMinusOne() || bq.isMinusOne()) {
      return Undefined::Expression();
    }

    Integer n = Integer::LogicalBitClear(aq, bq);
    Expression result = Rational::Builder(n);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression BitFlip::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));

    if(aq.isMinusOne() || bq.isMinusOne()) {
      return Undefined::Expression();
    }

    Integer n = Integer::LogicalBitFlip(aq, bq);
    Expression result = Rational::Builder(n);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression BitGet::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));

    if(aq.isMinusOne() || bq.isMinusOne()) {
      return Undefined::Expression();
    }

    Integer n = Integer::LogicalBitGet(aq, bq);
    Expression result = Rational::Builder(n);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression BitSet::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));

    if(aq.isMinusOne() || bq.isMinusOne()) {
      return Undefined::Expression();
    }

    Integer n = Integer::LogicalBitSet(aq, bq);
    Expression result = Rational::Builder(n);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression BitsClear::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));

    if(aq.isMinusOne() || bq.isMinusOne()) {
      return Undefined::Expression();
    }

    Integer n = Integer::LogicalBitsClear(aq, bq);
    Expression result = Rational::Builder(n);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression BitsClearExplicit::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));
    Integer cq = getValidInteger(childAtIndex(2));

    if(aq.isMinusOne() || bq.isMinusOne() || cq.isMinusOne() || !cq.isLowerThan(Integer(__INT8_MAX__))) {
      return Undefined::Expression();
    }

    Integer n = Integer::LogicalBitsClear(aq, bq, cq);
    Expression result = Rational::Builder(n);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression ShiftLogicLeft::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));

    if(aq.isMinusOne() || bq.isMinusOne()) {
      return Undefined::Expression();
    }

    Integer n = Integer::LogicalShiftLeft(aq, bq);
    Expression result = Rational::Builder(n);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression ShiftLogicLeftExplicit::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));
    Integer cq = getValidInteger(childAtIndex(2));

    if(aq.isMinusOne() || bq.isMinusOne() || cq.isMinusOne() || !cq.isLowerThan(Integer(__INT8_MAX__))) {
      return Undefined::Expression();
    }

    Integer n = Integer::LogicalShiftLeft(aq, bq, cq);
    Expression result = Rational::Builder(n);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression ShiftLogicRight::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));

    if(aq.isMinusOne() || bq.isMinusOne()) {
      return Undefined::Expression();
    }

    Integer n = Integer::LogicalShiftRight(aq, bq);
    Expression result = Rational::Builder(n);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression ShiftLogicRightExplicit::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));
    Integer cq = getValidInteger(childAtIndex(2));

    if(aq.isMinusOne() || bq.isMinusOne() || cq.isMinusOne() || !cq.isLowerThan(Integer(__INT8_MAX__))) {
      return Undefined::Expression();
    }

    Integer n = Integer::LogicalShiftRight(aq, bq, cq);
    Expression result = Rational::Builder(n);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression ShiftArithmeticRight::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));

    if(aq.isMinusOne() || bq.isMinusOne()) {
      return Undefined::Expression();
    }

    Integer n = Integer::LogicalShiftRightArithmetic(aq, bq);
    Expression result = Rational::Builder(n);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression ShiftArithmeticRightExplicit::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));
    Integer cq = getValidInteger(childAtIndex(2));

    if(aq.isMinusOne() || bq.isMinusOne() || cq.isMinusOne() || !cq.isLowerThan(Integer(__INT8_MAX__))) {
      return Undefined::Expression();
    }

    Integer n = Integer::LogicalShiftRightArithmetic(aq, bq, cq);
    Expression result = Rational::Builder(n);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression RotateLeft::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));

    if(aq.isMinusOne() || bq.isMinusOne()) {
      return Undefined::Expression();
    }

    Integer n = Integer::LogicalRotateLeft(aq, bq);
    Expression result = Rational::Builder(n);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression RotateLeftExplicit::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));
    Integer cq = getValidInteger(childAtIndex(2));

    if(aq.isMinusOne() || bq.isMinusOne() || cq.isMinusOne() || !cq.isLowerThan(Integer(__INT8_MAX__))) {
      return Undefined::Expression();
    }

    Integer n = Integer::LogicalRotateLeft(aq, bq, cq);
    Expression result = Rational::Builder(n);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression RotateRight::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));

    if(aq.isMinusOne() || bq.isMinusOne()) {
      return Undefined::Expression();
    }

    Integer n = Integer::LogicalRotateRight(aq, bq);
    Expression result = Rational::Builder(n);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression RotateRightExplicit::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));
    Integer cq = getValidInteger(childAtIndex(2));

    if(aq.isMinusOne() || bq.isMinusOne() || cq.isMinusOne() || !cq.isLowerThan(Integer(__INT8_MAX__))) {
      return Undefined::Expression();
    }

    Integer n = Integer::LogicalRotateRight(aq, bq, cq);
    Expression result = Rational::Builder(n);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

  Expression TwosComplement::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
    {
      Expression e = Expression::defaultShallowReduce();
      e = e.defaultHandleUnitsInChildren();
      if (e.isUndefined()) {
        return e;
      }
    }
    Integer aq = getValidInteger(childAtIndex(0));
    Integer bq = getValidInteger(childAtIndex(1));

    if(aq.isMinusOne() || bq.isMinusOne() || !bq.isLowerThan(Integer(__INT8_MAX__))) {
      return Undefined::Expression();
    }

    Integer n = Integer::TwosComplementToBits(aq, bq);
    Expression result = Rational::Builder(n);
    replaceWithInPlace(result);
    return result.shallowReduce(reductionContext);
  }

template int BinaryOperationNode<1>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<2>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<3>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<4>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<5>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<6>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<7>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<8>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<9>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<10>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<11>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<12>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<13>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<14>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<15>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<16>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<17>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<18>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<19>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<20>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<21>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<22>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<23>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<24>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<25>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<26>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<27>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<28>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<29>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<30>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int BinaryOperationNode<31>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;

} // namespace Poincare
