#include <poincare_expressions.h>
#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/expression/builtin.h>
#include <poincare_junior/src/expression/constant.h>
#include <poincare_junior/src/expression/float.h>
#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/expression/matrix.h>
#include <poincare_junior/src/expression/rational.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/expression/symbol.h>
#include <poincare_junior/src/layout/layoutter.h>
#include <poincare_junior/src/layout/parser.h>
#include <poincare_junior/src/layout/parsing/rack_parser.h>
#include <poincare_junior/src/memory/cache_pool.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/n_ary.h>

#include "poincare_junior/src/memory/type_block.h"

namespace PoincareJ {

Poincare::Expression ToPoincareExpressionViaParse(const Tree *exp) {
  EditionReference outputLayout = Layoutter::LayoutExpression(exp->clone());
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
  RackParser(inputLayout).parse();
  inputLayout->removeTree();
  return;
}

Poincare::Expression Expression::ToPoincareExpression(const Tree *exp) {
  // NOTE: Make sure new BlockTypes are handled here.
  BlockType type = exp->type();

  if (Builtin::IsReservedFunction(type)) {
    Poincare::Expression child = ToPoincareExpression(exp->child(0));
    switch (type) {
      case BlockType::SquareRoot:
        return Poincare::SquareRoot::Builder(child);
      case BlockType::NthRoot:
        return Poincare::NthRoot::Builder(child,
                                          ToPoincareExpression(exp->child(1)));
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
      case BlockType::Secant:
        return Poincare::Secant::Builder(child);
      case BlockType::Cosecant:
        return Poincare::Cosecant::Builder(child);
      case BlockType::Cotangent:
        return Poincare::Cotangent::Builder(child);
      case BlockType::ArcSecant:
        return Poincare::ArcSecant::Builder(child);
      case BlockType::ArcCosecant:
        return Poincare::ArcCosecant::Builder(child);
      case BlockType::ArcCotangent:
        return Poincare::ArcCotangent::Builder(child);
      case BlockType::HyperbolicCosine:
        return Poincare::HyperbolicCosine::Builder(child);
      case BlockType::HyperbolicSine:
        return Poincare::HyperbolicSine::Builder(child);
      case BlockType::HyperbolicTangent:
        return Poincare::HyperbolicTangent::Builder(child);
      case BlockType::HyperbolicArcCosine:
        return Poincare::HyperbolicArcCosine::Builder(child);
      case BlockType::HyperbolicArcSine:
        return Poincare::HyperbolicArcSine::Builder(child);
      case BlockType::HyperbolicArcTangent:
        return Poincare::HyperbolicArcTangent::Builder(child);
      case BlockType::Abs:
        return Poincare::AbsoluteValue::Builder(child);
      case BlockType::Log:
        return Poincare::Logarithm::Builder(child);
      case BlockType::Logarithm:
        return Poincare::Logarithm::Builder(
            child, ToPoincareExpression(exp->child(1)));
      case BlockType::Cross:
        return Poincare::VectorCross::Builder(
            child, ToPoincareExpression(exp->child(1)));
      case BlockType::Dot:
        return Poincare::VectorDot::Builder(
            child, ToPoincareExpression(exp->child(1)));
      case BlockType::Det:
        return Poincare::Determinant::Builder(child);
      case BlockType::Dim:
        return Poincare::Dimension::Builder(child);
      case BlockType::Identity:
        return Poincare::MatrixIdentity::Builder(child);
      case BlockType::Inverse:
        return Poincare::MatrixInverse::Builder(child);
      case BlockType::Ln:
        return Poincare::NaperianLogarithm::Builder(child);
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
      case BlockType::PercentSimple:
        return Poincare::PercentSimple::Builder(child);
      case BlockType::PercentAddition:
        return Poincare::PercentAddition::Builder(
            child, ToPoincareExpression(exp->child(1)));
      case BlockType::Derivative: {
        Poincare::Expression symbol = child;
        if (symbol.type() != Poincare::ExpressionNode::Type::Symbol) {
          return Poincare::Undefined::Builder();
        }
        return Poincare::Derivative::Builder(
            ToPoincareExpression(exp->child(2)),
            static_cast<Poincare::Symbol &>(symbol),
            ToPoincareExpression(exp->child(1)));
      }
      case BlockType::Integral: {
        Poincare::Expression symbol = child;
        if (symbol.type() != Poincare::ExpressionNode::Type::Symbol) {
          return Poincare::Undefined::Builder();
        }
        return Poincare::Integral::Builder(
            ToPoincareExpression(exp->child(3)),
            static_cast<Poincare::Symbol &>(symbol),
            ToPoincareExpression(exp->child(1)),
            ToPoincareExpression(exp->child(2)));
      }
      case BlockType::Sum: {
        Poincare::Expression symbol = child;
        if (symbol.type() != Poincare::ExpressionNode::Type::Symbol) {
          return Poincare::Undefined::Builder();
        }
        return Poincare::Sum::Builder(ToPoincareExpression(exp->child(3)),
                                      static_cast<Poincare::Symbol &>(symbol),
                                      ToPoincareExpression(exp->child(1)),
                                      ToPoincareExpression(exp->child(2)));
      }
      case BlockType::Product: {
        Poincare::Expression symbol = child;
        if (symbol.type() != Poincare::ExpressionNode::Type::Symbol) {
          return Poincare::Undefined::Builder();
        }
        return Poincare::Product::Builder(
            ToPoincareExpression(exp->child(3)),
            static_cast<Poincare::Symbol &>(symbol),
            ToPoincareExpression(exp->child(1)),
            ToPoincareExpression(exp->child(2)));
      }
      default:
        // TODO: Handle missing BlockTypes
        assert(false);
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
      Poincare::Expression child0 = ToPoincareExpression(exp->child(0));
      Poincare::Expression child1 = ToPoincareExpression(exp->child(1));
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
    case BlockType::SingleFloat:
    case BlockType::DoubleFloat:
    case BlockType::Decimal:
    case BlockType::Unit:
      return ToPoincareExpressionViaParse(exp);
    case BlockType::UserSymbol: {
      char buffer[20];
      Symbol::GetName(exp, buffer, std::size(buffer));
      return Poincare::Symbol::Builder(buffer, Symbol::Length(exp));
    }
    case BlockType::ComplexI:
      return Poincare::Constant::ComplexIBuilder();
    case BlockType::Constant: {
      if (Constant::Type(exp) == Constant::Type::Pi) {
        return Poincare::Constant::PiBuilder();
      }
      if (Constant::Type(exp) == Constant::Type::E) {
        return Poincare::Constant::ExponentialEBuilder();
      }
      return Poincare::Undefined::Builder();
    }
    case BlockType::Infinity:
      return Poincare::Infinity::Builder(false);
    case BlockType::Factorial:
      return Poincare::Factorial::Builder(ToPoincareExpression(exp->child(0)));
    case BlockType::Nonreal:
      return Poincare::Nonreal::Builder();
    case BlockType::Opposite:
      return Poincare::Opposite::Builder(ToPoincareExpression(exp->child(0)));
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
      SharedEditionPool->push(BlockType::Abs);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Opposite:
      SharedEditionPool->push<BlockType::Multiplication>(2);
      SharedEditionPool->push(BlockType::MinusOne);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::SquareRoot:
      SharedEditionPool->push(BlockType::SquareRoot);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::NthRoot:
      SharedEditionPool->push(BlockType::NthRoot);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::Cosine:
      SharedEditionPool->push(BlockType::Cosine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Sine:
      SharedEditionPool->push(BlockType::Sine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Tangent:
      SharedEditionPool->push(BlockType::Tangent);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcCosine:
      SharedEditionPool->push(BlockType::ArcCosine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcSine:
      SharedEditionPool->push(BlockType::ArcSine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcTangent:
      SharedEditionPool->push(BlockType::ArcTangent);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Cosecant:
      SharedEditionPool->push(BlockType::Cosecant);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Secant:
      SharedEditionPool->push(BlockType::Secant);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Cotangent:
      SharedEditionPool->push(BlockType::Cotangent);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcCosecant:
      SharedEditionPool->push(BlockType::ArcCosecant);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcSecant:
      SharedEditionPool->push(BlockType::ArcSecant);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcCotangent:
      SharedEditionPool->push(BlockType::ArcCotangent);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::HyperbolicCosine:
      SharedEditionPool->push(BlockType::HyperbolicCosine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::HyperbolicSine:
      SharedEditionPool->push(BlockType::HyperbolicSine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::HyperbolicTangent:
      SharedEditionPool->push(BlockType::HyperbolicTangent);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::HyperbolicArcCosine:
      SharedEditionPool->push(BlockType::HyperbolicArcCosine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::HyperbolicArcSine:
      SharedEditionPool->push(BlockType::HyperbolicArcSine);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::HyperbolicArcTangent:
      SharedEditionPool->push(BlockType::HyperbolicArcTangent);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::NaperianLogarithm:
      SharedEditionPool->push(BlockType::Ln);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Dimension:
      SharedEditionPool->push(BlockType::Dim);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Determinant:
      SharedEditionPool->push(BlockType::Det);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixIdentity:
      SharedEditionPool->push(BlockType::Identity);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixInverse:
      SharedEditionPool->push(BlockType::Inverse);
      return PushPoincareExpression(exp.childAtIndex(0));
    // case OT::: // FIXME Norm is AbsoluteValue
    // SharedEditionPool->push(BlockType::Norm);
    // return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixTrace:
      SharedEditionPool->push(BlockType::Trace);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixTranspose:
      SharedEditionPool->push(BlockType::Transpose);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixRowEchelonForm:
      SharedEditionPool->push(BlockType::Ref);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixReducedRowEchelonForm:
      SharedEditionPool->push(BlockType::Rref);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::VectorCross:
      SharedEditionPool->push(BlockType::Cross);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::VectorDot:
      SharedEditionPool->push(BlockType::Dot);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::ComplexArgument:
      SharedEditionPool->push(BlockType::ComplexArgument);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Conjugate:
      SharedEditionPool->push(BlockType::Conjugate);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ImaginaryPart:
      SharedEditionPool->push(BlockType::ImaginaryPart);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::RealPart:
      SharedEditionPool->push(BlockType::RealPart);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::PercentSimple:
      SharedEditionPool->push(BlockType::PercentSimple);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::PercentAddition:
      SharedEditionPool->push(BlockType::PercentAddition);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::Logarithm:
      if (exp.numberOfChildren() == 2) {
        SharedEditionPool->push(BlockType::Logarithm);
        PushPoincareExpression(exp.childAtIndex(0));
        PushPoincareExpression(exp.childAtIndex(1));
      } else {
        assert(exp.numberOfChildren() == 1);
        SharedEditionPool->push(BlockType::Log);
        PushPoincareExpression(exp.childAtIndex(0));
      }
      return;
    case OT::Derivative:
      SharedEditionPool->push(BlockType::Derivative);
      PushPoincareExpression(exp.childAtIndex(1));
      PushPoincareExpression(exp.childAtIndex(2));
      PushPoincareExpression(exp.childAtIndex(0));
      return;
    case OT::Integral:
      SharedEditionPool->push(BlockType::Integral);
      PushPoincareExpression(exp.childAtIndex(1));
      PushPoincareExpression(exp.childAtIndex(2));
      PushPoincareExpression(exp.childAtIndex(3));
      PushPoincareExpression(exp.childAtIndex(0));
      return;
    case OT::Sum:
    case OT::Product:
      SharedEditionPool->push(exp.type() == OT::Sum ? BlockType::Sum
                                                    : BlockType::Product);
      PushPoincareExpression(exp.childAtIndex(1));
      PushPoincareExpression(exp.childAtIndex(2));
      PushPoincareExpression(exp.childAtIndex(3));
      PushPoincareExpression(exp.childAtIndex(0));
      return;
    case OT::Comparison:
      // TODO: Handle comparisons better
      assert(Poincare::ComparisonNode::IsBinaryEquality(exp));
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
        case OT::Comparison:
        case OT::Subtraction:
          SharedEditionPool->push(BlockType::Subtraction);
          break;
        case OT::Division:
          SharedEditionPool->push(BlockType::Division);
          break;
        case OT::Power:
          SharedEditionPool->push(BlockType::Power);
          break;
        case OT::Matrix:
          SharedEditionPool->push<BlockType::Matrix>(
              static_cast<Poincare::Matrix &>(exp).numberOfRows(),
              static_cast<Poincare::Matrix &>(exp).numberOfColumns());
          break;
        default:
          assert(false);
      }
      for (int i = 0; i < exp.numberOfChildren(); i++) {
        PushPoincareExpression(exp.childAtIndex(i));
      }
      return;
    case OT::Nonreal:
      SharedEditionPool->push(BlockType::Nonreal);
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
        SharedEditionPool->push(BlockType::ComplexI);
      } else {
        SharedEditionPool->push(BlockType::Undefined);
      }
      return;
    }
    case OT::Infinity: {
      if (exp.isPositive(nullptr) == Poincare::TrinaryBoolean::False) {
        SharedEditionPool->push<BlockType::Multiplication>(2);
        SharedEditionPool->push(BlockType::MinusOne);
      }
      SharedEditionPool->push(BlockType::Infinity);
    }
    default:
      SharedEditionPool->push(BlockType::Undefined);
  }
}

Tree *Expression::FromPoincareExpression(Poincare::Expression exp) {
  Tree *node = Tree::FromBlocks(SharedEditionPool->lastBlock());
  PushPoincareExpression(exp);
  return node;
}

}  // namespace PoincareJ
