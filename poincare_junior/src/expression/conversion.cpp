#include <poincare_expressions.h>
#include <poincare_junior/include/expression.h>
#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/builtin.h>
#include <poincare_junior/src/expression/constant.h>
#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/expression/rational.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/expression/symbol.h>
#include <poincare_junior/src/layout/parser.h>
#include <poincare_junior/src/memory/cache_pool.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/n_ary.h>

namespace PoincareJ {

Poincare::Expression Expression::ToPoincareExpression(const Node *exp) {
  BlockType type = exp->type();

  if (Builtin::IsBuiltin(type)) {
    Poincare::Expression child = ToPoincareExpression(exp->childAtIndex(0));
    switch (type) {
      case BlockType::SquareRoot:
        return Poincare::SquareRoot::Builder(child);
      case BlockType::Cosine:
        return Poincare::Cosine::Builder(child);
      case BlockType::Sine:
        return Poincare::Sine::Builder(child);
      case BlockType::Tangent:
        return Poincare::Tangent::Builder(child);
      case BlockType::ArcCosine:
        return Poincare::ArcCosine::Builder(child);
      case BlockType::ArcSine:
        return Poincare::ArcSine::Builder(child);
      case BlockType::ArcTangent:
        return Poincare::ArcTangent::Builder(child);
      case BlockType::Abs:
        return Poincare::AbsoluteValue::Builder(child);
      case BlockType::Log:
        return Poincare::Logarithm::Builder(child);
      case BlockType::Logarithm:
        return Poincare::Logarithm::Builder(
            child, ToPoincareExpression(exp->childAtIndex(1)));
      case BlockType::Derivative: {
        Poincare::Expression symbol =
            ToPoincareExpression(exp->childAtIndex(1));
        if (symbol.type() != Poincare::ExpressionNode::Type::Symbol) {
          return Poincare::Undefined::Builder();
        }
        return Poincare::Derivative::Builder(
            child, static_cast<Poincare::Symbol &>(symbol),
            ToPoincareExpression(exp->childAtIndex(2)));
      }
    }
  }

  switch (type) {
    case BlockType::Addition:
    case BlockType::Multiplication: {
      Poincare::NAryExpression nary =
          type == BlockType::Addition ? static_cast<Poincare::NAryExpression>(
                                            Poincare::Addition::Builder())
                                      : Poincare::Multiplication::Builder();
      for (const Node *child : exp->children()) {
        nary.addChildAtIndexInPlace(ToPoincareExpression(child),
                                    nary.numberOfChildren(),
                                    nary.numberOfChildren());
      }
      return nary;
    }
    case BlockType::Subtraction:
    case BlockType::Power:
    case BlockType::Division: {
      Poincare::Expression child0 = ToPoincareExpression(exp->childAtIndex(0));
      Poincare::Expression child1 = ToPoincareExpression(exp->childAtIndex(1));
      Poincare::Expression result;
      if (type == BlockType::Subtraction) {
        result = Poincare::Subtraction::Builder(child0, child1);
      } else if (type == BlockType::Division) {
        result = Poincare::Division::Builder(child0, child1);
      } else {
        result = Poincare::Power::Builder(child0, child1);
      }
      return result;
    }
    case BlockType::Zero:
    case BlockType::MinusOne:
    case BlockType::One:
    case BlockType::Two:
    case BlockType::IntegerShort:
    case BlockType::IntegerPosBig:
    case BlockType::IntegerNegBig:
      return Poincare::Rational::Builder(Integer::Handler(exp).to<double>());
    case BlockType::Half:
    case BlockType::RationalShort:
    case BlockType::RationalPosBig:
    case BlockType::RationalNegBig:
      return Poincare::Rational::Builder(
          Rational::Numerator(exp).to<double>(),
          Rational::Denominator(exp).to<double>());
    case BlockType::Float:
      return Poincare::Float<double>::Builder(Approximation::To<double>(exp));
    case BlockType::Ln:
      return Poincare::NaperianLogarithm::Builder(
          ToPoincareExpression(exp->childAtIndex(0)));
    case BlockType::UserSymbol: {
      char buffer[20];
      Symbol::GetName(exp, buffer, std::size(buffer));
      return Poincare::Symbol::Builder(buffer, Symbol::Length(exp));
    }
    case BlockType::Constant: {
      if (Constant::Type(exp) == Constant::Type::Pi) {
        return Poincare::Constant::PiBuilder();
      }
      if (Constant::Type(exp) == Constant::Type::E) {
        return Poincare::Constant::ExponentialEBuilder();
      }
      return Poincare::Undefined::Builder();
    }
    case BlockType::Factorial:
      return Poincare::Factorial::Builder(
          ToPoincareExpression(exp->childAtIndex(0)));
    case BlockType::UserFunction:
    case BlockType::UserSequence:
    case BlockType::Set:
    case BlockType::List:
    case BlockType::Polynomial:
    default:
      return Poincare::Undefined::Builder();
  }
}

void Expression::PushPoincareExpression(Poincare::Expression exp) {
  using OT = Poincare::ExpressionNode::Type;
  switch (exp.type()) {
    case OT::Parenthesis:
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::AbsoluteValue:
      editionPool->pushBlock(BlockType::Abs);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Opposite:
      editionPool->push<BlockType::Multiplication>(2);
      editionPool->pushBlock(BlockType::MinusOne);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::SquareRoot:
      editionPool->pushBlock(BlockType::SquareRoot);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Cosine:
      editionPool->pushBlock(BlockType::Cosine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Sine:
      editionPool->pushBlock(BlockType::Sine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Tangent:
      editionPool->pushBlock(BlockType::Tangent);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcCosine:
      editionPool->pushBlock(BlockType::ArcCosine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcSine:
      editionPool->pushBlock(BlockType::ArcSine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcTangent:
      editionPool->pushBlock(BlockType::ArcTangent);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::NaperianLogarithm:
      editionPool->pushBlock(BlockType::Ln);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Logarithm:
      if (exp.numberOfChildren() == 2) {
        editionPool->pushBlock(BlockType::Logarithm);
        PushPoincareExpression(exp.childAtIndex(0));
        PushPoincareExpression(exp.childAtIndex(1));
      } else {
        assert(exp.numberOfChildren() == 1);
        editionPool->pushBlock(BlockType::Log);
        PushPoincareExpression(exp.childAtIndex(0));
      }
      return;
    case OT::Derivative:
      editionPool->pushBlock(BlockType::Derivative);
      PushPoincareExpression(exp.childAtIndex(0));
      PushPoincareExpression(exp.childAtIndex(1));
      PushPoincareExpression(exp.childAtIndex(2));
      return;
    case OT::Addition:
    case OT::Multiplication:
    case OT::Subtraction:
    case OT::Division:
    case OT::Power:
      switch (exp.type()) {
        case OT::Addition:
          editionPool->push<BlockType::Addition>(exp.numberOfChildren());
          break;
        case OT::Multiplication:
          editionPool->push<BlockType::Multiplication>(exp.numberOfChildren());
          break;
        case OT::Subtraction:
          editionPool->pushBlock(BlockType::Subtraction);
          break;
        case OT::Division:
          editionPool->pushBlock(BlockType::Division);
          break;
        case OT::Power:
          editionPool->pushBlock(BlockType::Power);
          break;
      }
      for (int i = 0; i < exp.numberOfChildren(); i++) {
        PushPoincareExpression(exp.childAtIndex(i));
      }
      return;
    case OT::Rational: {
      Poincare::Rational rat = static_cast<Poincare::Rational &>(exp);
      int num = rat.signedIntegerNumerator().approximate<double>();
      int den = rat.integerDenominator().approximate<double>();
      Rational::Push(IntegerHandler(num), IntegerHandler(den));
      return;
    }
    case OT::BasedInteger: {
      Poincare::BasedInteger i = static_cast<Poincare::BasedInteger &>(exp);
      int num = i.doubleApproximation();
      Rational::Push(IntegerHandler(num), IntegerHandler(1));
      return;
    }
    case OT::Float: {
      Poincare::Float<float> f = static_cast<Poincare::Float<float> &>(exp);
      editionPool->push<BlockType::Float>(f.value());
      return;
    }
    case OT::Symbol: {
      Poincare::Symbol s = static_cast<Poincare::Symbol &>(exp);
      editionPool->push<BlockType::UserSymbol>(s.name(), strlen(s.name()));
      return;
    }
    case OT::ConstantMaths: {
      Poincare::Constant c = static_cast<Poincare::Constant &>(exp);
      if (c.isExponentialE()) {
        editionPool->push<BlockType::Constant>(u'e');
      } else if (c.isPi()) {
        editionPool->push<BlockType::Constant>(u'π');
      } else {
        editionPool->pushBlock(BlockType::Undefined);
      }
      return;
    }
    default:
      editionPool->pushBlock(BlockType::Undefined);
  }
}

Node *Expression::FromPoincareExpression(Poincare::Expression exp) {
  Node *node = Node::FromBlocks(editionPool->lastBlock());
  PushPoincareExpression(exp);
  return node;
}

}  // namespace PoincareJ
