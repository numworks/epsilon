#include "conversion.h"

#include <poincare_expressions.h>
#include <poincare_junior/src/expression/builtin.h>
#include <poincare_junior/src/expression/float.h>
#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/expression/matrix.h>
#include <poincare_junior/src/expression/rational.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/expression/symbol.h>
#include <poincare_junior/src/layout/layoutter.h>
#include <poincare_junior/src/layout/parser.h>
#include <poincare_junior/src/layout/parsing/rack_parser.h>
#include <poincare_junior/src/layout/rack_from_text.h>
#include <poincare_junior/src/layout/serialize.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/n_ary.h>

#include "poincare_junior/src/memory/type_block.h"

namespace PoincareJ {

Poincare::OExpression ToPoincareExpressionViaParse(const Tree *exp) {
  EditionReference outputLayout = Layoutter::LayoutExpression(exp->clone());
  constexpr size_t bufferSize = 256;
  char buffer[bufferSize];
  *Serialize(outputLayout, buffer, buffer + bufferSize) = 0;
  outputLayout->removeTree();
  return Poincare::OExpression::Parse(buffer, nullptr, false, false);
}

void PushPoincareExpressionViaParse(Poincare::OExpression exp) {
  constexpr size_t bufferSize = 256;
  char buffer[bufferSize];
  exp.serialize(buffer, bufferSize);
  EditionReference inputLayout = RackFromText(buffer);
  RackParser(inputLayout).parse();
  inputLayout->removeTree();
  return;
}

Poincare::ComparisonNode::OperatorType ComparisonToOperator(BlockType type) {
  switch (type) {
    case BlockType::Equal:
      return Poincare::ComparisonNode::OperatorType::Equal;
    case BlockType::NotEqual:
      return Poincare::ComparisonNode::OperatorType::NotEqual;
    case BlockType::Superior:
      return Poincare::ComparisonNode::OperatorType::Superior;
    case BlockType::Inferior:
      return Poincare::ComparisonNode::OperatorType::Inferior;
    case BlockType::SuperiorEqual:
      return Poincare::ComparisonNode::OperatorType::SuperiorEqual;
    case BlockType::InferiorEqual:
      return Poincare::ComparisonNode::OperatorType::InferiorEqual;
    default:
      assert(false);
  }
}

Poincare::OExpression ToPoincareExpression(const Tree *exp) {
  // NOTE: Make sure new BlockTypes are handled here.
  BlockType type = exp->type();

  if (Builtin::IsReservedFunction(exp)) {
    Poincare::OExpression child = ToPoincareExpression(exp->child(0));
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
      case BlockType::Ceiling:
        return Poincare::Ceiling::Builder(child);
      case BlockType::Floor:
        return Poincare::Floor::Builder(child);
      case BlockType::FracPart:
        return Poincare::FracPart::Builder(child);
      case BlockType::Log:
        return Poincare::Logarithm::Builder(child);
      case BlockType::Logarithm:
        return Poincare::Logarithm::Builder(
            child, ToPoincareExpression(exp->child(1)));
      case BlockType::Binomial:
        return Poincare::BinomialCoefficient::Builder(
            child, ToPoincareExpression(exp->child(1)));
      case BlockType::Permute:
        return Poincare::PermuteCoefficient::Builder(
            child, ToPoincareExpression(exp->child(1)));
      case BlockType::Round:
        return Poincare::Round::Builder(child,
                                        ToPoincareExpression(exp->child(1)));
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
        Poincare::OExpression symbol = child;
        if (symbol.type() != Poincare::ExpressionNode::Type::Symbol) {
          return Poincare::Undefined::Builder();
        }
        return Poincare::Derivative::Builder(
            ToPoincareExpression(exp->child(2)),
            static_cast<Poincare::Symbol &>(symbol),
            ToPoincareExpression(exp->child(1)));
      }
      case BlockType::Integral: {
        Poincare::OExpression symbol = child;
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
        Poincare::OExpression symbol = child;
        if (symbol.type() != Poincare::ExpressionNode::Type::Symbol) {
          return Poincare::Undefined::Builder();
        }
        return Poincare::Sum::Builder(ToPoincareExpression(exp->child(3)),
                                      static_cast<Poincare::Symbol &>(symbol),
                                      ToPoincareExpression(exp->child(1)),
                                      ToPoincareExpression(exp->child(2)));
      }
      case BlockType::Product: {
        Poincare::OExpression symbol = child;
        if (symbol.type() != Poincare::ExpressionNode::Type::Symbol) {
          return Poincare::Undefined::Builder();
        }
        return Poincare::Product::Builder(
            ToPoincareExpression(exp->child(3)),
            static_cast<Poincare::Symbol &>(symbol),
            ToPoincareExpression(exp->child(1)),
            ToPoincareExpression(exp->child(2)));
      }
      case BlockType::Dependency: {
        assert(exp->child(1)->isSet());
        Poincare::OList listOfDependencies = Poincare::OList::Builder();
        for (const Tree *child : exp->child(1)->children()) {
          listOfDependencies.addChildAtIndexInPlace(ToPoincareExpression(child),
                                                    0, 0);
        }
        return Poincare::Dependency::Builder(
            ToPoincareExpression(exp->child(0)), listOfDependencies);
      }
      case BlockType::Piecewise: {
        Poincare::List arguments = Poincare::List::Builder();
        int i = 0;
        for (const Tree *child : exp->children()) {
          arguments.addChildAtIndexInPlace(ToPoincareExpression(child), i, i);
          i++;
        }
        return Poincare::PiecewiseOperator::UntypedBuilder(arguments);
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
      Poincare::OMatrix mat = Poincare::OMatrix::Builder();
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
      Poincare::OExpression child0 = ToPoincareExpression(exp->child(0));
      Poincare::OExpression child1 = ToPoincareExpression(exp->child(1));
      Poincare::OExpression result;
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
    case BlockType::Pi:
      return Poincare::Constant::PiBuilder();
    case BlockType::ExponentialE:
      return Poincare::Constant::ExponentialEBuilder();
    case BlockType::Infinity:
      return Poincare::Infinity::Builder(false);
    case BlockType::Factorial:
      return Poincare::Factorial::Builder(ToPoincareExpression(exp->child(0)));
    case BlockType::Nonreal:
      return Poincare::Nonreal::Builder();
    case BlockType::Opposite:
      return Poincare::Opposite::Builder(ToPoincareExpression(exp->child(0)));
    case BlockType::Equal:
    case BlockType::NotEqual:
    case BlockType::Superior:
    case BlockType::Inferior:
    case BlockType::SuperiorEqual:
    case BlockType::InferiorEqual:
      return Poincare::Comparison::Builder(ToPoincareExpression(exp->child(0)),
                                           ComparisonToOperator(type),
                                           ToPoincareExpression(exp->child(1)));
    case BlockType::UserFunction:
    case BlockType::UserSequence:
    case BlockType::Set:
    case BlockType::List:
    case BlockType::Polynomial:
    default:
      return Poincare::Undefined::Builder();
  }
}

void PushPoincareExpression(Poincare::OExpression exp) {
  using OT = Poincare::ExpressionNode::Type;
  switch (exp.type()) {
    case OT::AbsoluteValue:
      SharedEditionPool->push(BlockType::Abs);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Ceiling:
      SharedEditionPool->push(BlockType::Ceiling);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Floor:
      SharedEditionPool->push(BlockType::Floor);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::FracPart:
      SharedEditionPool->push(BlockType::FracPart);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Factorial:
      SharedEditionPool->push(BlockType::Factorial);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Opposite:
      SharedEditionPool->push(BlockType::Opposite);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::SignFunction:
      SharedEditionPool->push(BlockType::Sign);
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
    case OT::BinomialCoefficient:
      SharedEditionPool->push(BlockType::Binomial);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::PermuteCoefficient:
      SharedEditionPool->push(BlockType::Permute);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::Round:
      SharedEditionPool->push(BlockType::Round);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::VectorCross:
      SharedEditionPool->push(BlockType::Cross);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::VectorDot:
      SharedEditionPool->push(BlockType::Dot);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::VectorNorm:
      SharedEditionPool->push(BlockType::Norm);
      return PushPoincareExpression(exp.childAtIndex(0));
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
    case OT::Factor:
      SharedEditionPool->push(BlockType::Factor);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::PercentSimple:
      SharedEditionPool->push(BlockType::PercentSimple);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::PercentAddition:
      SharedEditionPool->push(BlockType::PercentAddition);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::DivisionQuotient:
      SharedEditionPool->push(BlockType::Quotient);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::DivisionRemainder:
      SharedEditionPool->push(BlockType::Remainder);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::ListElement:
      SharedEditionPool->push(BlockType::ListElement);
      // list is last in poincare
      PushPoincareExpression(exp.childAtIndex(1));
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ListSlice:
      SharedEditionPool->push(BlockType::ListSlice);
      // list is last in poincare
      PushPoincareExpression(exp.childAtIndex(2));
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::ListSort:
      SharedEditionPool->push(BlockType::ListSort);
      return PushPoincareExpression(exp.childAtIndex(0));

    case OT::ListMean:
    case OT::ListVariance:
    case OT::ListStandardDeviation:
    case OT::ListSampleStandardDeviation:
    case OT::ListMedian: {
      switch (exp.type()) {
        case OT::ListMean:
          SharedEditionPool->push(BlockType::Mean);
          break;
        case OT::ListVariance:
          SharedEditionPool->push(BlockType::Variance);
          break;
        case OT::ListSampleStandardDeviation:
          SharedEditionPool->push(BlockType::SampleStdDev);
          break;
        case OT::ListStandardDeviation:
          SharedEditionPool->push(BlockType::StdDev);
          break;
        case OT::ListMedian:
          SharedEditionPool->push(BlockType::Median);
          break;
        case OT::ListSum:
          SharedEditionPool->push(BlockType::ListSum);
          break;
        case OT::ListProduct:
          SharedEditionPool->push(BlockType::ListProduct);
          break;
      }
      PushPoincareExpression(exp.childAtIndex(0));
      if (exp.numberOfChildren() == 2) {
        PushPoincareExpression(exp.childAtIndex(1));
      } else {
        (1_e)->clone();
      }
      return;
    }
    case OT::ListSum:
      SharedEditionPool->push(BlockType::ListSum);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ListProduct:
      SharedEditionPool->push(BlockType::ListProduct);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ListMinimum:
      SharedEditionPool->push(BlockType::Minimum);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ListMaximum:
      SharedEditionPool->push(BlockType::Maximum);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Store:
      SharedEditionPool->push(BlockType::Store);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::UnitConvert:
      SharedEditionPool->push(BlockType::UnitConversion);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::Boolean:
      SharedEditionPool->push(static_cast<Poincare::Boolean &>(exp).value()
                                  ? BlockType::True
                                  : BlockType::False);
      return;
    case OT::LogicalOperatorNot:
      SharedEditionPool->push(BlockType::LogicalNot);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::BinaryLogicalOperator: {
      Poincare::BinaryLogicalOperator op =
          static_cast<Poincare::BinaryLogicalOperator &>(exp);
      BlockType type;
      switch (op.operatorType()) {
        case Poincare::BinaryLogicalOperatorNode::OperatorType::And:
          type = BlockType::LogicalAnd;
          break;
        case Poincare::BinaryLogicalOperatorNode::OperatorType::Or:
          type = BlockType::LogicalOr;
          break;
        case Poincare::BinaryLogicalOperatorNode::OperatorType::Xor:
          type = BlockType::LogicalXor;
          break;
        case Poincare::BinaryLogicalOperatorNode::OperatorType::Nand:
          type = BlockType::LogicalNand;
          break;
        case Poincare::BinaryLogicalOperatorNode::OperatorType::Nor:
          type = BlockType::LogicalNor;
          break;
        default:
          assert(false);
      }
      SharedEditionPool->push(type);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    }
    case OT::Point:
      SharedEditionPool->push(BlockType::Point);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::MixedFraction:
      SharedEditionPool->push(BlockType::MixedFraction);
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
    case OT::ListSequence:
      SharedEditionPool->push(BlockType::ListSequence);
      PushPoincareExpression(exp.childAtIndex(1));
      PushPoincareExpression(exp.childAtIndex(2));
      PushPoincareExpression(exp.childAtIndex(0));
      return;
    case OT::Derivative:
      if (exp.childAtIndex(3).isOne()) {
        SharedEditionPool->push(BlockType::Derivative);
        PushPoincareExpression(exp.childAtIndex(1));
        PushPoincareExpression(exp.childAtIndex(2));
        PushPoincareExpression(exp.childAtIndex(0));
      } else {
        SharedEditionPool->push(BlockType::NthDerivative);
        PushPoincareExpression(exp.childAtIndex(1));
        PushPoincareExpression(exp.childAtIndex(2));
        PushPoincareExpression(exp.childAtIndex(3));
        PushPoincareExpression(exp.childAtIndex(0));
      }
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
      if (exp.numberOfChildren() > 2) {
        // This will not work semantically but is sufficient for createLayout
        Poincare::Comparison c = static_cast<Poincare::Comparison &>(exp);
        Poincare::OExpression e = Poincare::Comparison::Builder(
            Poincare::Comparison::Builder(
                c.childAtIndex(0), c.operatorAtIndex(0), c.childAtIndex(1)),
            c.operatorAtIndex(1), c.childAtIndex(2));
        return PushPoincareExpression(e);
      }
      // TODO: Handle comparisons better
      assert(exp.numberOfChildren() == 2);
    case OT::Addition:
    case OT::PiecewiseOperator:
    case OT::Multiplication:
    case OT::List:
    case OT::Subtraction:
    case OT::Division:
    case OT::Power:
    case OT::Matrix:
    case OT::GreatCommonDivisor:
    case OT::LeastCommonMultiple:
      switch (exp.type()) {
        case OT::Addition:
          SharedEditionPool->push<BlockType::Addition>(exp.numberOfChildren());
          break;
        case OT::Multiplication:
          SharedEditionPool->push<BlockType::Multiplication>(
              exp.numberOfChildren());
          break;
        case OT::List:
          SharedEditionPool->push<BlockType::List>(exp.numberOfChildren());
          break;
        case OT::PiecewiseOperator:
          SharedEditionPool->push<BlockType::Piecewise>(exp.numberOfChildren());
          break;
        case OT::GreatCommonDivisor:
          SharedEditionPool->push(BlockType::GCD);
          SharedEditionPool->push(exp.numberOfChildren());
          break;
        case OT::LeastCommonMultiple:
          SharedEditionPool->push(BlockType::LCM);
          SharedEditionPool->push(exp.numberOfChildren());
          break;
        case OT::Comparison: {
          Poincare::Comparison c = static_cast<Poincare::Comparison &>(exp);
          BlockType type;
          switch (c.operatorAtIndex(0)) {
            case Poincare::ComparisonNode::OperatorType::Equal:
              type = BlockType::Equal;
              break;
            case Poincare::ComparisonNode::OperatorType::NotEqual:
              type = BlockType::NotEqual;
              break;
            case Poincare::ComparisonNode::OperatorType::Inferior:
              type = BlockType::Inferior;
              break;
            case Poincare::ComparisonNode::OperatorType::InferiorEqual:
              type = BlockType::InferiorEqual;
              break;
            case Poincare::ComparisonNode::OperatorType::Superior:
              type = BlockType::Superior;
              break;
            case Poincare::ComparisonNode::OperatorType::SuperiorEqual:
              type = BlockType::SuperiorEqual;
              break;
            default:
              assert(false);
          }
          SharedEditionPool->push(type);
          break;
        }
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
              static_cast<Poincare::OMatrix &>(exp).numberOfRows(),
              static_cast<Poincare::OMatrix &>(exp).numberOfColumns());
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
    case OT::Unit:
    case OT::ConstantPhysics:
      return PushPoincareExpressionViaParse(exp);
    case OT::Decimal: {
      Poincare::Decimal d = static_cast<Poincare::Decimal &>(exp);
      if (d.node()->isNegative()) {
        SharedEditionPool->push(BlockType::Opposite);
      }
      int numberOfDigits = Poincare::Integer::NumberOfBase10DigitsWithoutSign(
          d.node()->unsignedMantissa());
      int8_t exponent = numberOfDigits - 1 - d.node()->exponent();
      SharedEditionPool->push<BlockType::Decimal>(exponent);
      Poincare::Integer mantissa = d.node()->unsignedMantissa();
      char buffer[100];
      mantissa.serialize(buffer, 100);
      UTF8Decoder decoder(buffer);
      IntegerHandler::Parse(decoder, OMG::Base::Decimal).pushOnEditionPool();
      return;
    }
    case OT::Sequence:
    case OT::Function:
    case OT::Symbol: {
      Poincare::Symbol s = static_cast<Poincare::Symbol &>(exp);
      Tree *t = SharedEditionPool->push<BlockType::UserSymbol>(
          s.name(), exp.type() == OT::Sequence ? 1 : strlen(s.name()));
      if (exp.type() == OT::Function) {
        *t->block() = BlockType::UserFunction;
        PushPoincareExpression(exp.childAtIndex(0));
      }
      if (exp.type() == OT::Sequence) {
        *t->block() = BlockType::UserSequence;
        PushPoincareExpression(exp.childAtIndex(0));
      }
      return;
    }
    case OT::ConstantMaths: {
      Poincare::Constant c = static_cast<Poincare::Constant &>(exp);
      if (c.isExponentialE()) {
        SharedEditionPool->push(BlockType::ExponentialE);
      } else if (c.isPi()) {
        SharedEditionPool->push(BlockType::Pi);
      } else if (c.isComplexI()) {
        SharedEditionPool->push(BlockType::ComplexI);
      } else {
        SharedEditionPool->push(BlockType::Undefined);
      }
      return;
    }
    case OT::Float:
      SharedEditionPool->push<BlockType::SingleFloat>(
          static_cast<Poincare::Float<float> &>(exp).value());
      return;
    case OT::Double:
      SharedEditionPool->push<BlockType::DoubleFloat>(
          static_cast<Poincare::Float<double> &>(exp).value());
      return;
    case OT::Infinity: {
      if (exp.isPositive(nullptr) == Poincare::TrinaryBoolean::False) {
        SharedEditionPool->push(BlockType::Opposite);
      }
      SharedEditionPool->push(BlockType::Infinity);
      return;
    }
    case OT::DistributionDispatcher: {
      SharedEditionPool->push(BlockType::Distribution);
      SharedEditionPool->push(exp.numberOfChildren());
      Poincare::DistributionDispatcher dd =
          static_cast<Poincare::DistributionDispatcher &>(exp);
      SharedEditionPool->push(static_cast<uint8_t>(dd.distributionType()));
      SharedEditionPool->push(static_cast<uint8_t>(dd.methodType()));
      for (int i = 0; i < exp.numberOfChildren(); i++) {
        PushPoincareExpression(exp.childAtIndex(i));
      }
      return;
    }
    case OT::Random:
      SharedEditionPool->push(BlockType::Random);
      SharedEditionPool->push(0);  // seed
      return;
    case OT::Randint:
      SharedEditionPool->push(BlockType::RandInt);
      SharedEditionPool->push(0);  // seed
      if (exp.numberOfChildren() == 2) {
        PushPoincareExpression(exp.childAtIndex(0));
        PushPoincareExpression(exp.childAtIndex(1));
      } else {
        (1_e)->clone();
        PushPoincareExpression(exp.childAtIndex(1));
      }
      return;
    case OT::RandintNoRepeat:
      SharedEditionPool->push(BlockType::RandIntNoRep);
      SharedEditionPool->push(0);  // seed
      PushPoincareExpression(exp.childAtIndex(0));
      PushPoincareExpression(exp.childAtIndex(1));
      PushPoincareExpression(exp.childAtIndex(2));
      return;
    case OT::Undefined:
      SharedEditionPool->push(BlockType::Undefined);
      return;
    case OT::EmptyExpression:
      SharedEditionPool->push(BlockType::Empty);
      return;
    case OT::Parenthesis:
      SharedEditionPool->push(BlockType::Parenthesis);
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::JuniorExpression: {
      SharedEditionPool->clone(
          static_cast<Poincare::JuniorExpression &>(exp).tree());
      return;
    }
    default:
      assert(false);
  }
}

Tree *FromPoincareExpression(Poincare::OExpression exp) {
  Tree *node = Tree::FromBlocks(SharedEditionPool->lastBlock());
  PushPoincareExpression(exp);
  return node;
}

}  // namespace PoincareJ
