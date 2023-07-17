#include <poincare_expressions.h>
#include <poincare_junior/include/expression.h>
#include <poincare_junior/src/expression/builtin.h>
#include <poincare_junior/src/expression/constant.h>
#include <poincare_junior/src/expression/float.h>
#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/expression/rational.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/expression/symbol.h>
#include <poincare_junior/src/layout/parser.h>
#include <poincare_junior/src/layout/parsing/rack_parser.h>
#include <poincare_junior/src/memory/cache_pool.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/n_ary.h>

namespace PoincareJ {

Poincare::Expression LazyToPoincareExpression(const Tree *exp) {
  EditionReference outputLayout =
      Expression::EditionPoolExpressionToLayout(exp->clone());
  constexpr size_t bufferSize = 256;
  char buffer[bufferSize];
  *Layout::Serialize(outputLayout, buffer, buffer + bufferSize) = 0;
  outputLayout->removeTree();
  return Poincare::Expression::Parse(buffer, nullptr, false, false);
}

void LazyPushPoincareExpression(Poincare::Expression exp) {
  constexpr size_t bufferSize = 256;
  char buffer[bufferSize];
  exp.serialize(buffer, bufferSize);
  EditionReference inputLayout = Layout::EditionPoolTextToLayout(buffer);
  EditionReference expression = RackParser(inputLayout).parse();
  inputLayout->removeTree();
  return;
}

Poincare::Expression Expression::ToPoincareExpression(const Tree *exp) {
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
      for (const Tree *child : exp->children()) {
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
    case BlockType::Half:
    case BlockType::RationalShort:
    case BlockType::RationalPosBig:
    case BlockType::RationalNegBig:
    case BlockType::Float:
    case BlockType::Decimal:
      return LazyToPoincareExpression(exp);
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
      SharedEditionPool->pushBlock(BlockType::Abs);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Opposite:
      SharedEditionPool->push<BlockType::Multiplication>(2);
      SharedEditionPool->pushBlock(BlockType::MinusOne);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::SquareRoot:
      SharedEditionPool->pushBlock(BlockType::SquareRoot);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Cosine:
      SharedEditionPool->pushBlock(BlockType::Cosine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Sine:
      SharedEditionPool->pushBlock(BlockType::Sine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Tangent:
      SharedEditionPool->pushBlock(BlockType::Tangent);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcCosine:
      SharedEditionPool->pushBlock(BlockType::ArcCosine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcSine:
      SharedEditionPool->pushBlock(BlockType::ArcSine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcTangent:
      SharedEditionPool->pushBlock(BlockType::ArcTangent);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::NaperianLogarithm:
      SharedEditionPool->pushBlock(BlockType::Ln);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Logarithm:
      if (exp.numberOfChildren() == 2) {
        SharedEditionPool->pushBlock(BlockType::Logarithm);
        PushPoincareExpression(exp.childAtIndex(0));
        PushPoincareExpression(exp.childAtIndex(1));
      } else {
        assert(exp.numberOfChildren() == 1);
        SharedEditionPool->pushBlock(BlockType::Log);
        PushPoincareExpression(exp.childAtIndex(0));
      }
      return;
    case OT::Derivative:
      SharedEditionPool->pushBlock(BlockType::Derivative);
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
          SharedEditionPool->push<BlockType::Addition>(exp.numberOfChildren());
          break;
        case OT::Multiplication:
          SharedEditionPool->push<BlockType::Multiplication>(
              exp.numberOfChildren());
          break;
        case OT::Subtraction:
          SharedEditionPool->pushBlock(BlockType::Subtraction);
          break;
        case OT::Division:
          SharedEditionPool->pushBlock(BlockType::Division);
          break;
        case OT::Power:
          SharedEditionPool->pushBlock(BlockType::Power);
          break;
      }
      for (int i = 0; i < exp.numberOfChildren(); i++) {
        PushPoincareExpression(exp.childAtIndex(i));
      }
      return;
    case OT::Rational:
    case OT::BasedInteger:
    case OT::Float:
    case OT::Decimal:
      return LazyPushPoincareExpression(exp);
    case OT::Symbol: {
      Poincare::Symbol s = static_cast<Poincare::Symbol &>(exp);
      SharedEditionPool->push<BlockType::UserSymbol>(s.name(),
                                                     strlen(s.name()));
      return;
    }
    case OT::ConstantMaths: {
      Poincare::Constant c = static_cast<Poincare::Constant &>(exp);
      if (c.isExponentialE()) {
        SharedEditionPool->push<BlockType::Constant>(u'e');
      } else if (c.isPi()) {
        SharedEditionPool->push<BlockType::Constant>(u'π');
      } else {
        SharedEditionPool->pushBlock(BlockType::Undefined);
      }
      return;
    }
    default:
      SharedEditionPool->pushBlock(BlockType::Undefined);
  }
}

Tree *Expression::FromPoincareExpression(Poincare::Expression exp) {
  Tree *node = Tree::FromBlocks(SharedEditionPool->lastBlock());
  PushPoincareExpression(exp);
  return node;
}

}  // namespace PoincareJ
