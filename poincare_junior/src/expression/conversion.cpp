#include <poincare_expressions.h>
#include <poincare_junior/include/expression.h>
#include <poincare_junior/src/expression/builtin.h>
#include <poincare_junior/src/expression/constant.h>
#include <poincare_junior/src/expression/float.h>
#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/expression/matrix.h>
#include <poincare_junior/src/expression/rational.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/expression/symbol.h>
#include <poincare_junior/src/layout/parser.h>
#include <poincare_junior/src/layout/parsing/rack_parser.h>
#include <poincare_junior/src/memory/cache_pool.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/n_ary.h>

namespace PoincareJ {

Poincare::Expression ToPoincareExpressionViaParse(const Tree *exp) {
  EditionReference outputLayout =
      Expression::EditionPoolExpressionToLayout(exp->clone());
  constexpr size_t bufferSize = 256;
  char buffer[bufferSize];
  *Layout::Serialize(outputLayout, buffer, buffer + bufferSize) = 0;
  outputLayout->removeTree();
  return Poincare::Expression::Parse(buffer, nullptr, false, false);
}

void PushPoincareExpressionViaParse(Poincare::Expression exp) {
  constexpr size_t bufferSize = 256;
  char buffer[bufferSize];
  exp.serialize(buffer, bufferSize);
  EditionReference inputLayout = Layout::EditionPoolTextToLayout(buffer);
  EditionReference expression = RackParser(inputLayout).parse();
  inputLayout->removeTree();
  return;
}

Poincare::Expression Expression::ToPoincareExpression(const Tree *exp) {
  // NOTE: Make sure new BlockTypes are handled here.
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
      case BlockType::Cross:
        return Poincare::VectorCross::Builder(
            child, ToPoincareExpression(exp->childAtIndex(1)));
      case BlockType::Dot:
        return Poincare::VectorDot::Builder(
            child, ToPoincareExpression(exp->childAtIndex(1)));
      case BlockType::Det:
        return Poincare::Determinant::Builder(child);
      case BlockType::Dim:
        return Poincare::Dimension::Builder(child);
      case BlockType::Identity:
        return Poincare::MatrixIdentity::Builder(child);
      case BlockType::Inverse:
        return Poincare::MatrixInverse::Builder(child);
      case BlockType::Norm:
        return Poincare::AbsoluteValue::Builder(child);
      case BlockType::Ref:
        return Poincare::MatrixRowEchelonForm::Builder(child);
      case BlockType::Rref:
        return Poincare::MatrixReducedRowEchelonForm::Builder(child);
      case BlockType::Trace:
        return Poincare::MatrixTrace::Builder(child);
      case BlockType::Transpose:
        return Poincare::MatrixTranspose::Builder(child);
      case BlockType::ComplexArgument:
        return Poincare::ComplexArgument::Builder(child);
      case BlockType::Conjugate:
        return Poincare::Conjugate::Builder(child);
      case BlockType::ImaginaryPart:
        return Poincare::ImaginaryPart::Builder(child);
      case BlockType::RealPart:
        return Poincare::RealPart::Builder(child);
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
    case BlockType::Matrix: {
      Poincare::Matrix mat = Poincare::Matrix::Builder();
      for (const Tree *child : exp->children()) {
        mat.addChildAtIndexInPlace(ToPoincareExpression(child),
                                   mat.numberOfChildren(),
                                   mat.numberOfChildren());
      }
      mat.setDimensions(Matrix::NumberOfRows(exp),
                        Matrix::NumberOfColumns(exp));
      return mat;
    }
    case BlockType::Subtraction:
    case BlockType::Power:
    case BlockType::PowerMatrix:
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
    case BlockType::Unit:
      return ToPoincareExpressionViaParse(exp);
    case BlockType::Ln:
      return Poincare::NaperianLogarithm::Builder(
          ToPoincareExpression(exp->childAtIndex(0)));
    case BlockType::UserSymbol: {
      char buffer[20];
      Symbol::GetName(exp, buffer, std::size(buffer));
      return Poincare::Symbol::Builder(buffer, Symbol::Length(exp));
    }
    case BlockType::Constant: {
      if (Constant::Type(exp) == Constant::Type::I) {
        return Poincare::Constant::ComplexIBuilder();
      }
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
    case OT::Dimension:
      SharedEditionPool->pushBlock(BlockType::Dim);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Determinant:
      SharedEditionPool->pushBlock(BlockType::Det);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixIdentity:
      SharedEditionPool->pushBlock(BlockType::Identity);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixInverse:
      SharedEditionPool->pushBlock(BlockType::Inverse);
      return PushPoincareExpression(exp.childAtIndex(0));
    // case OT::: // FIXME Norm is AbsoluteValue
    // SharedEditionPool->pushBlock(BlockType::Norm);
    // return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixTrace:
      SharedEditionPool->pushBlock(BlockType::Trace);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixTranspose:
      SharedEditionPool->pushBlock(BlockType::Transpose);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixRowEchelonForm:
      SharedEditionPool->pushBlock(BlockType::Ref);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixReducedRowEchelonForm:
      SharedEditionPool->pushBlock(BlockType::Rref);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::VectorCross:
      SharedEditionPool->pushBlock(BlockType::Cross);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::VectorDot:
      SharedEditionPool->pushBlock(BlockType::Dot);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::ComplexArgument:
      SharedEditionPool->pushBlock(BlockType::ComplexArgument);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Conjugate:
      SharedEditionPool->pushBlock(BlockType::Conjugate);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ImaginaryPart:
      SharedEditionPool->pushBlock(BlockType::ImaginaryPart);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::RealPart:
      SharedEditionPool->pushBlock(BlockType::RealPart);
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
    case OT::Matrix:
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
        case OT::Matrix:
          SharedEditionPool->push<BlockType::Matrix>(
              static_cast<Poincare::Matrix &>(exp).numberOfRows(),
              static_cast<Poincare::Matrix &>(exp).numberOfColumns());
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
    case OT::Unit:
      return PushPoincareExpressionViaParse(exp);
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
      } else if (c.isComplexI()) {
        SharedEditionPool->push<BlockType::Constant>(u'i');
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
