#include "conversion.h"

#include <omg/unreachable.h>
#include <poincare/old/poincare_expressions.h>
#include <poincare/src/layout/layouter.h>
#include <poincare/src/layout/parser.h>
#include <poincare/src/layout/parsing/rack_parser.h>
#include <poincare/src/layout/rack_from_text.h>
#include <poincare/src/layout/serialize.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/tree_ref.h>
#include <poincare/src/memory/type_block.h>

#include "builtin.h"
#include "dependency.h"
#include "float.h"
#include "integer.h"
#include "matrix.h"
#include "rational.h"
#include "simplification.h"
#include "symbol.h"

#define DISABLE_CONVERSIONS 0

namespace Poincare::Internal {

Poincare::OExpression ToPoincareExpressionViaParse(const Tree* exp) {
  TreeRef outputLayout = Layouter::LayoutExpression(exp->cloneTree());
  constexpr size_t bufferSize = 256;
  char buffer[bufferSize];
  *Serialize(outputLayout, buffer, buffer + bufferSize) = 0;
  outputLayout->removeTree();
  OMG::unreachable();
  // we may use the new parse here
  // return Poincare::OExpression::Parse(buffer, nullptr, false, false);
}

void PushPoincareExpressionViaParse(Poincare::OExpression exp) {
  constexpr size_t bufferSize = 256;
  char buffer[bufferSize];
  exp.serialize(buffer, bufferSize);
  TreeRef inputLayout = RackFromText(buffer);
  RackParser(inputLayout, nullptr).parse();
  inputLayout->removeTree();
  return;
}

Poincare::ComparisonNode::OperatorType ComparisonToOperator(Type type) {
  switch (type) {
    case Type::Equal:
      return Poincare::ComparisonNode::OperatorType::Equal;
    case Type::NotEqual:
      return Poincare::ComparisonNode::OperatorType::NotEqual;
    case Type::Superior:
      return Poincare::ComparisonNode::OperatorType::Superior;
    case Type::Inferior:
      return Poincare::ComparisonNode::OperatorType::Inferior;
    case Type::SuperiorEqual:
      return Poincare::ComparisonNode::OperatorType::SuperiorEqual;
    case Type::InferiorEqual:
      return Poincare::ComparisonNode::OperatorType::InferiorEqual;
    default:
      OMG::unreachable();
  }
}

Poincare::OExpression ToPoincareExpression(const Tree* e) {
#if DISABLE_CONVERSIONS
  assert(false);
#else
  // NOTE: Make sure new Types are handled here.
  Type type = e->type();

  if (Builtin::IsReservedFunction(e)) {
    Poincare::OExpression child = ToPoincareExpression(e->child(0));
    switch (type) {
      case Type::Sqrt:
        return Poincare::SquareRoot::Builder(child);
      case Type::Root:
        return Poincare::NthRoot::Builder(child,
                                          ToPoincareExpression(e->child(1)));
      case Type::Cos:
        return Poincare::Cosine::Builder(child);
      case Type::Sin:
        return Poincare::Sine::Builder(child);
      case Type::Tan:
        return Poincare::Tangent::Builder(child);
      case Type::ACos:
        return Poincare::ArcCosine::Builder(child);
      case Type::ASin:
        return Poincare::ArcSine::Builder(child);
      case Type::ATan:
        return Poincare::ArcTangent::Builder(child);
      case Type::Sec:
        return Poincare::Secant::Builder(child);
      case Type::Csc:
        return Poincare::Cosecant::Builder(child);
      case Type::Cot:
        return Poincare::Cotangent::Builder(child);
      case Type::ASec:
        return Poincare::ArcSecant::Builder(child);
      case Type::ACsc:
        return Poincare::ArcCosecant::Builder(child);
      case Type::ACot:
        return Poincare::ArcCotangent::Builder(child);
      case Type::CosH:
        return Poincare::HyperbolicCosine::Builder(child);
      case Type::SinH:
        return Poincare::HyperbolicSine::Builder(child);
      case Type::TanH:
        return Poincare::HyperbolicTangent::Builder(child);
      case Type::ArCosH:
        return Poincare::HyperbolicArcCosine::Builder(child);
      case Type::ArSinH:
        return Poincare::HyperbolicArcSine::Builder(child);
      case Type::ArTanH:
        return Poincare::HyperbolicArcTangent::Builder(child);
      case Type::Abs:
        return Poincare::AbsoluteValue::Builder(child);
      case Type::Ceil:
        return Poincare::Ceiling::Builder(child);
      case Type::Floor:
        return Poincare::Floor::Builder(child);
      case Type::Frac:
        return Poincare::FracPart::Builder(child);
      case Type::Log:
        return Poincare::Logarithm::Builder(child);
      case Type::LogBase:
        return Poincare::Logarithm::Builder(child,
                                            ToPoincareExpression(e->child(1)));
      case Type::Binomial:
        return Poincare::BinomialCoefficient::Builder(
            child, ToPoincareExpression(e->child(1)));
      case Type::Permute:
        return Poincare::PermuteCoefficient::Builder(
            child, ToPoincareExpression(e->child(1)));
      case Type::Round:
        return Poincare::Round::Builder(child,
                                        ToPoincareExpression(e->child(1)));
      case Type::Cross:
        return Poincare::VectorCross::Builder(
            child, ToPoincareExpression(e->child(1)));
      case Type::Dot:
        return Poincare::VectorDot::Builder(child,
                                            ToPoincareExpression(e->child(1)));
      case Type::Det:
        return Poincare::Determinant::Builder(child);
      case Type::Dim:
        return Poincare::ODimension::Builder(child);
      case Type::Identity:
        return Poincare::MatrixIdentity::Builder(child);
      case Type::Inverse:
        return Poincare::MatrixInverse::Builder(child);
      case Type::Ln:
        return Poincare::NaperianLogarithm::Builder(child);
      case Type::Norm:
        return Poincare::AbsoluteValue::Builder(child);
      case Type::Ref:
        return Poincare::MatrixRowEchelonForm::Builder(child);
      case Type::Rref:
        return Poincare::MatrixReducedRowEchelonForm::Builder(child);
      case Type::Trace:
        return Poincare::MatrixTrace::Builder(child);
      case Type::Transpose:
        return Poincare::MatrixTranspose::Builder(child);
      case Type::Arg:
        return Poincare::ComplexArgument::Builder(child);
      case Type::Conj:
        return Poincare::Conjugate::Builder(child);
      case Type::Im:
        return Poincare::ImaginaryPart::Builder(child);
      case Type::Re:
        return Poincare::RealPart::Builder(child);
      case Type::PercentSimple:
        return Poincare::PercentSimple::Builder(child);
      case Type::PercentAddition:
        return Poincare::PercentAddition::Builder(
            child, ToPoincareExpression(e->child(1)));
      case Type::Integral: {
        Poincare::OExpression symbol = child;
        if (symbol.otype() != Poincare::ExpressionNode::Type::Symbol) {
          return Poincare::Undefined::Builder();
        }
        return Poincare::Integral::Builder(
            ToPoincareExpression(e->child(3)),
            static_cast<Poincare::Symbol&>(symbol),
            ToPoincareExpression(e->child(1)),
            ToPoincareExpression(e->child(2)));
      }
      case Type::Sum: {
        Poincare::OExpression symbol = child;
        if (symbol.otype() != Poincare::ExpressionNode::Type::Symbol) {
          return Poincare::Undefined::Builder();
        }
        return Poincare::Sum::Builder(ToPoincareExpression(e->child(3)),
                                      static_cast<Poincare::Symbol&>(symbol),
                                      ToPoincareExpression(e->child(1)),
                                      ToPoincareExpression(e->child(2)));
      }
      case Type::Product: {
        Poincare::OExpression symbol = child;
        if (symbol.otype() != Poincare::ExpressionNode::Type::Symbol) {
          return Poincare::Undefined::Builder();
        }
        return Poincare::Product::Builder(
            ToPoincareExpression(e->child(3)),
            static_cast<Poincare::Symbol&>(symbol),
            ToPoincareExpression(e->child(1)),
            ToPoincareExpression(e->child(2)));
      }
      case Type::Dependency: {
        assert(Dependency::Dependencies(e)->isDependencies());
        Poincare::OList listOfDependencies = Poincare::OList::Builder();
        for (const Tree* child : Dependency::Dependencies(e)->children()) {
          listOfDependencies.addChildAtIndexInPlace(ToPoincareExpression(child),
                                                    0, 0);
        }
        return Poincare::Dependency::Builder(
            ToPoincareExpression(Dependency::Main(e)), listOfDependencies);
      }
      case Type::Piecewise: {
        Poincare::List arguments = Poincare::List::Builder();
        int i = 0;
        for (const Tree* child : e->children()) {
          arguments.addChildAtIndexInPlace(ToPoincareExpression(child), i, i);
          i++;
        }
        return Poincare::PiecewiseOperator::UntypedBuilder(arguments);
      }
      default:
        // Unhandled types.
        assert(false);
    }
  }

  switch (type) {
    case Type::Add:
    case Type::Mult: {
      Poincare::NAryExpression nary =
          type == Type::Add ? static_cast<Poincare::NAryExpression>(
                                  Poincare::Addition::Builder())
                            : Poincare::Multiplication::Builder();
      for (const Tree* child : e->children()) {
        nary.addChildAtIndexInPlace(ToPoincareExpression(child),
                                    nary.numberOfChildren(),
                                    nary.numberOfChildren());
      }
      return nary;
    }
    case Type::List: {
      Poincare::OList list = Poincare::OList::Builder();
      for (const Tree* child : e->children()) {
        list.addChildAtIndexInPlace(ToPoincareExpression(child),
                                    list.numberOfChildren(),
                                    list.numberOfChildren());
      }
      return list;
    }
    case Type::Matrix: {
      Poincare::OMatrix mat = Poincare::OMatrix::Builder();
      for (const Tree* child : e->children()) {
        mat.addChildAtIndexInPlace(ToPoincareExpression(child),
                                   mat.numberOfChildren(),
                                   mat.numberOfChildren());
      }
      mat.setDimensions(Matrix::NumberOfRows(e), Matrix::NumberOfColumns(e));
      return mat;
    }
    case Type::Sub:
    case Type::Pow:
    case Type::PowMatrix:
    case Type::Div: {
      Poincare::OExpression child0 = ToPoincareExpression(e->child(0));
      Poincare::OExpression child1 = ToPoincareExpression(e->child(1));
      Poincare::OExpression result;
      if (type == Type::Sub) {
        result = Poincare::Subtraction::Builder(child0, child1);
      } else if (type == Type::Div) {
        result = Poincare::Division::Builder(child0, child1);
      } else {
        result = Poincare::Power::Builder(child0, child1);
      }
      return result;
    }
    case Type::Zero:
    case Type::MinusOne:
    case Type::One:
    case Type::Two:
    case Type::IntegerPosShort:
    case Type::IntegerNegShort:
    case Type::IntegerPosBig:
    case Type::IntegerNegBig:
    case Type::Half:
    case Type::RationalPosShort:
    case Type::RationalNegShort:
    case Type::RationalPosBig:
    case Type::RationalNegBig:
    case Type::SingleFloat:
    case Type::DoubleFloat:
    case Type::Decimal:
    case Type::Unit:
      return ToPoincareExpressionViaParse(e);
    case Type::UserSymbol: {
      return Poincare::Symbol::Builder(Symbol::GetName(e), Symbol::Length(e));
    }
    case Type::ComplexI:
      return Poincare::Constant::ComplexIBuilder();
    case Type::Pi:
      return Poincare::Constant::PiBuilder();
    case Type::EulerE:
      return Poincare::Constant::ExponentialEBuilder();
    case Type::Inf:
      return Poincare::Infinity::Builder(false);
    case Type::Fact:
      return Poincare::Factorial::Builder(ToPoincareExpression(e->child(0)));
    case Type::NonReal:
      return Poincare::Nonreal::Builder();
    case Type::Opposite:
      return Poincare::Opposite::Builder(ToPoincareExpression(e->child(0)));
    case Type::Equal:
    case Type::NotEqual:
    case Type::Superior:
    case Type::Inferior:
    case Type::SuperiorEqual:
    case Type::InferiorEqual:
      return Poincare::Comparison::Builder(ToPoincareExpression(e->child(0)),
                                           ComparisonToOperator(type),
                                           ToPoincareExpression(e->child(1)));
    case Type::UserFunction:
      return Poincare::Function::Builder(Symbol::GetName(e), Symbol::Length(e),
                                         ToPoincareExpression(e->child(0)));
    case Type::UserSequence:
      return Poincare::Sequence::Builder(Symbol::GetName(e), Symbol::Length(e),
                                         ToPoincareExpression(e->child(0)));
    case Type::Set:
    case Type::Dependencies:
    case Type::Polynomial:
    default:
      return Poincare::Undefined::Builder();
  }
#endif
}

void PushPoincareExpression(Poincare::OExpression exp) {
#if DISABLE_CONVERSIONS
  assert(false);
#else
  using OT = Poincare::ExpressionNode::Type;
  switch (exp.otype()) {
    case OT::AbsoluteValue:
      SharedTreeStack->pushAbs();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Ceiling:
      SharedTreeStack->pushCeil();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Floor:
      SharedTreeStack->pushFloor();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::FracPart:
      SharedTreeStack->pushFrac();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Factorial:
      SharedTreeStack->pushFact();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Opposite:
      SharedTreeStack->pushOpposite();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::SignFunction:
      SharedTreeStack->pushSign();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::SquareRoot:
      SharedTreeStack->pushSqrt();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::NthRoot:
      SharedTreeStack->pushRoot();
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::Cosine:
      SharedTreeStack->pushCos();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Sine:
      SharedTreeStack->pushSin();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Tangent:
      SharedTreeStack->pushTan();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcCosine:
      SharedTreeStack->pushACos();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcSine:
      SharedTreeStack->pushASin();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcTangent:
      SharedTreeStack->pushATan();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Cosecant:
      SharedTreeStack->pushCsc();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Secant:
      SharedTreeStack->pushSec();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Cotangent:
      SharedTreeStack->pushCot();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcCosecant:
      SharedTreeStack->pushACsc();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcSecant:
      SharedTreeStack->pushASec();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ArcCotangent:
      SharedTreeStack->pushACot();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::HyperbolicCosine:
      SharedTreeStack->pushCosH();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::HyperbolicSine:
      SharedTreeStack->pushSinH();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::HyperbolicTangent:
      SharedTreeStack->pushTanH();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::HyperbolicArcCosine:
      SharedTreeStack->pushArCosH();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::HyperbolicArcSine:
      SharedTreeStack->pushArSinH();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::HyperbolicArcTangent:
      SharedTreeStack->pushArTanH();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::NaperianLogarithm:
      SharedTreeStack->pushLn();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Dimension:
      SharedTreeStack->pushDim();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Determinant:
      SharedTreeStack->pushDet();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixIdentity:
      SharedTreeStack->pushIdentity();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixInverse:
      SharedTreeStack->pushInverse();
      return PushPoincareExpression(exp.childAtIndex(0));
    // case OT::: // FIXME Norm is AbsoluteValue
    // SharedTreeStack->pushNorm();
    // return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixTrace:
      SharedTreeStack->pushTrace();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixTranspose:
      SharedTreeStack->pushTranspose();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixRowEchelonForm:
      SharedTreeStack->pushRef();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::MatrixReducedRowEchelonForm:
      SharedTreeStack->pushRref();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::BinomialCoefficient:
      SharedTreeStack->pushBinomial();
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::PermuteCoefficient:
      SharedTreeStack->pushPermute();
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::Round:
      SharedTreeStack->pushRound();
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::VectorCross:
      SharedTreeStack->pushCross();
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::VectorDot:
      SharedTreeStack->pushDot();
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::VectorNorm:
      SharedTreeStack->pushNorm();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ComplexArgument:
      SharedTreeStack->pushArg();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Conjugate:
      SharedTreeStack->pushConj();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ImaginaryPart:
      SharedTreeStack->pushIm();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::RealPart:
      SharedTreeStack->pushRe();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Factor:
      SharedTreeStack->pushFactor();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::PercentSimple:
      SharedTreeStack->pushPercentSimple();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::PercentAddition:
      SharedTreeStack->pushPercentAddition();
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::DivisionQuotient:
      SharedTreeStack->pushQuo();
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::DivisionRemainder:
      SharedTreeStack->pushRem();
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::ListElement:
      SharedTreeStack->pushListElement();
      // list is last in poincare
      PushPoincareExpression(exp.childAtIndex(1));
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ListSlice:
      SharedTreeStack->pushListSlice();
      // list is last in poincare
      PushPoincareExpression(exp.childAtIndex(2));
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::ListSort:
      SharedTreeStack->pushListSort();
      return PushPoincareExpression(exp.childAtIndex(0));

    case OT::ListMean:
    case OT::ListVariance:
    case OT::ListStandardDeviation:
    case OT::ListSampleStandardDeviation:
    case OT::ListMedian: {
      switch (exp.otype()) {
        case OT::ListMean:
          SharedTreeStack->pushMean();
          break;
        case OT::ListVariance:
          SharedTreeStack->pushVariance();
          break;
        case OT::ListSampleStandardDeviation:
          SharedTreeStack->pushSampleStdDev();
          break;
        case OT::ListStandardDeviation:
          SharedTreeStack->pushStdDev();
          break;
        case OT::ListMedian:
          SharedTreeStack->pushMedian();
          break;
        case OT::ListSum:
          SharedTreeStack->pushListSum();
          break;
        case OT::ListProduct:
          SharedTreeStack->pushListProduct();
          break;
        default:
          assert(false);
      }
      PushPoincareExpression(exp.childAtIndex(0));
      if (exp.numberOfChildren() == 2) {
        PushPoincareExpression(exp.childAtIndex(1));
      } else {
        (1_e)->cloneTree();
      }
      return;
    }
    case OT::ListSum:
      SharedTreeStack->pushListSum();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ListProduct:
      SharedTreeStack->pushListProduct();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ListMinimum:
      SharedTreeStack->pushMin();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::ListMaximum:
      SharedTreeStack->pushMax();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::Store:
      SharedTreeStack->pushStore();
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::UnitConvert:
      SharedTreeStack->pushUnitConversion();
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::OBoolean:
      SharedTreeStack->pushBlock(static_cast<Poincare::OBoolean&>(exp).value()
                                     ? Type::True
                                     : Type::False);
      return;
    case OT::LogicalOperatorNot:
      SharedTreeStack->pushLogicalNot();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::BinaryLogicalOperator: {
      Poincare::BinaryLogicalOperator op =
          static_cast<Poincare::BinaryLogicalOperator&>(exp);
      Type type;
      switch (op.operatorType()) {
        case Poincare::BinaryLogicalOperatorNode::OperatorType::And:
          type = Type::LogicalAnd;
          break;
        case Poincare::BinaryLogicalOperatorNode::OperatorType::Or:
          type = Type::LogicalOr;
          break;
        case Poincare::BinaryLogicalOperatorNode::OperatorType::Xor:
          type = Type::LogicalXor;
          break;
        case Poincare::BinaryLogicalOperatorNode::OperatorType::Nand:
          type = Type::LogicalNand;
          break;
        case Poincare::BinaryLogicalOperatorNode::OperatorType::Nor:
          type = Type::LogicalNor;
          break;
        default:
          OMG::unreachable();
      }
      SharedTreeStack->pushBlock(type);
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    }
    case OT::OPoint:
      SharedTreeStack->pushPoint();
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::MixedFraction:
      SharedTreeStack->pushMixedFraction();
      PushPoincareExpression(exp.childAtIndex(0));
      return PushPoincareExpression(exp.childAtIndex(1));
    case OT::Logarithm:
      if (exp.numberOfChildren() == 2) {
        SharedTreeStack->pushLogBase();
        PushPoincareExpression(exp.childAtIndex(0));
        PushPoincareExpression(exp.childAtIndex(1));
      } else {
        assert(exp.numberOfChildren() == 1);
        SharedTreeStack->pushLog();
        PushPoincareExpression(exp.childAtIndex(0));
      }
      return;
    case OT::ListSequence:
      SharedTreeStack->pushListSequence();
      PushPoincareExpression(exp.childAtIndex(1));
      PushPoincareExpression(exp.childAtIndex(2));
      PushPoincareExpression(exp.childAtIndex(0));
      return;
    case OT::Derivative:
      SharedTreeStack->pushDiff();
      PushPoincareExpression(exp.childAtIndex(1));
      PushPoincareExpression(exp.childAtIndex(2));
      PushPoincareExpression(exp.childAtIndex(3));
      PushPoincareExpression(exp.childAtIndex(0));
      return;
    case OT::Integral:
      SharedTreeStack->pushIntegral();
      PushPoincareExpression(exp.childAtIndex(1));
      PushPoincareExpression(exp.childAtIndex(2));
      PushPoincareExpression(exp.childAtIndex(3));
      PushPoincareExpression(exp.childAtIndex(0));
      return;
    case OT::Sum:
    case OT::Product:
      SharedTreeStack->pushBlock(exp.otype() == OT::Sum ? Type::Sum
                                                        : Type::Product);
      PushPoincareExpression(exp.childAtIndex(1));
      PushPoincareExpression(exp.childAtIndex(2));
      PushPoincareExpression(exp.childAtIndex(3));
      PushPoincareExpression(exp.childAtIndex(0));
      return;
    case OT::Comparison:
      if (exp.numberOfChildren() > 2) {
        // This will not work semantically but is sufficient for createLayout
        Poincare::Comparison c = static_cast<Poincare::Comparison&>(exp);
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
    case OT::OList:
    case OT::Subtraction:
    case OT::Division:
    case OT::Power:
    case OT::OMatrix:
    case OT::GreatCommonDivisor:
    case OT::LeastCommonMultiple:
      switch (exp.otype()) {
        case OT::Addition:
          SharedTreeStack->pushAdd(exp.numberOfChildren());
          break;
        case OT::Multiplication:
          SharedTreeStack->pushMult(exp.numberOfChildren());
          break;
        case OT::OList:
          SharedTreeStack->pushList(exp.numberOfChildren());
          break;
        case OT::PiecewiseOperator:
          SharedTreeStack->pushPiecewise(exp.numberOfChildren());
          break;
        case OT::GreatCommonDivisor:
          SharedTreeStack->pushGCD(exp.numberOfChildren());
          break;
        case OT::LeastCommonMultiple:
          SharedTreeStack->pushLCM(exp.numberOfChildren());
          break;
        case OT::Comparison: {
          Poincare::Comparison c = static_cast<Poincare::Comparison&>(exp);
          Type type;
          switch (c.operatorAtIndex(0)) {
            case Poincare::ComparisonNode::OperatorType::Equal:
              type = Type::Equal;
              break;
            case Poincare::ComparisonNode::OperatorType::NotEqual:
              type = Type::NotEqual;
              break;
            case Poincare::ComparisonNode::OperatorType::Inferior:
              type = Type::Inferior;
              break;
            case Poincare::ComparisonNode::OperatorType::InferiorEqual:
              type = Type::InferiorEqual;
              break;
            case Poincare::ComparisonNode::OperatorType::Superior:
              type = Type::Superior;
              break;
            case Poincare::ComparisonNode::OperatorType::SuperiorEqual:
              type = Type::SuperiorEqual;
              break;
            default:
              OMG::unreachable();
          }
          SharedTreeStack->pushBlock(type);
          break;
        }
        case OT::Subtraction:
          SharedTreeStack->pushSub();
          break;
        case OT::Division:
          SharedTreeStack->pushDiv();
          break;
        case OT::Power:
          SharedTreeStack->pushPow();
          break;
        case OT::OMatrix:
          SharedTreeStack->pushMatrix(
              static_cast<Poincare::OMatrix&>(exp).numberOfRows(),
              static_cast<Poincare::OMatrix&>(exp).numberOfColumns());
          break;
        default:
          assert(false);
      }
      for (int i = 0; i < exp.numberOfChildren(); i++) {
        PushPoincareExpression(exp.childAtIndex(i));
      }
      return;
    case OT::Nonreal:
      SharedTreeStack->pushNonReal();
      return;
    case OT::Rational:
    case OT::BasedInteger:
    case OT::OUnit:
    case OT::ConstantPhysics:
      return PushPoincareExpressionViaParse(exp);
    case OT::Decimal: {
      Poincare::Decimal d = static_cast<Poincare::Decimal&>(exp);
      if (d.node()->isNegative()) {
        SharedTreeStack->pushOpposite();
      }
      int numberOfDigits = Poincare::Integer::NumberOfBase10DigitsWithoutSign(
          d.node()->unsignedMantissa());
      int8_t exponent = numberOfDigits - 1 - d.node()->exponent();
      SharedTreeStack->pushDecimal();
      Poincare::Integer mantissa = d.node()->unsignedMantissa();
      char buffer[100];
      mantissa.serialize(buffer, 100);
      UTF8Decoder decoder(buffer);
      IntegerHandler::Parse(decoder, OMG::Base::Decimal).pushOnTreeStack();
      Integer::Push(exponent);
      return;
    }
    case OT::Sequence:
    case OT::Function:
    case OT::Symbol: {
      Poincare::Symbol s = static_cast<Poincare::Symbol&>(exp);
      Tree* t = SharedTreeStack->pushUserSymbol(
          s.name(), (exp.otype() == OT::Sequence ? 1 : strlen(s.name())) + 1);
      if (exp.otype() == OT::Function) {
        *t->block() = Type::UserFunction;
        PushPoincareExpression(exp.childAtIndex(0));
      }
      if (exp.otype() == OT::Sequence) {
        *t->block() = Type::UserSequence;
        PushPoincareExpression(exp.childAtIndex(0));
      }
      return;
    }
    case OT::ConstantMaths: {
      Poincare::Constant c = static_cast<Poincare::Constant&>(exp);
      if (c.isExponentialE()) {
        SharedTreeStack->pushEulerE();
      } else if (c.isPi()) {
        SharedTreeStack->pushPi();
      } else if (c.isComplexI()) {
        SharedTreeStack->pushComplexI();
      } else {
        KUndefUnhandled->cloneTree();
      }
      return;
    }
    case OT::Float:
      SharedTreeStack->pushSingleFloat(
          static_cast<Poincare::Float<float>&>(exp).value());
      return;
    case OT::Double:
      SharedTreeStack->pushDoubleFloat(
          static_cast<Poincare::Float<double>&>(exp).value());
      return;
    case OT::Infinity: {
      if (exp.isPositive(nullptr) == OMG::Troolean::False) {
        SharedTreeStack->pushOpposite();
      }
      SharedTreeStack->pushInf();
      return;
    }
    case OT::DistributionDispatcher: {
      SharedTreeStack->pushBlock(Type::Distribution);
      SharedTreeStack->pushBlock(exp.numberOfChildren());
      Poincare::DistributionDispatcher dd =
          static_cast<Poincare::DistributionDispatcher&>(exp);
      SharedTreeStack->pushBlock(static_cast<uint8_t>(dd.distributionType()));
      SharedTreeStack->pushBlock(static_cast<uint8_t>(dd.methodType()));
      for (int i = 0; i < exp.numberOfChildren(); i++) {
        PushPoincareExpression(exp.childAtIndex(i));
      }
      return;
    }
    case OT::Random:
      SharedTreeStack->pushRandom(0);
      return;
    case OT::Randint:
      SharedTreeStack->pushRandInt(0);
      if (exp.numberOfChildren() == 2) {
        PushPoincareExpression(exp.childAtIndex(0));
        PushPoincareExpression(exp.childAtIndex(1));
      } else {
        (1_e)->cloneTree();
        PushPoincareExpression(exp.childAtIndex(1));
      }
      return;
    case OT::RandintNoRepeat:
      SharedTreeStack->pushRandIntNoRep(0);
      PushPoincareExpression(exp.childAtIndex(0));
      PushPoincareExpression(exp.childAtIndex(1));
      PushPoincareExpression(exp.childAtIndex(2));
      return;
    case OT::Undefined:
      KUndef->cloneTree();
      return;
    case OT::EmptyExpression:
      SharedTreeStack->pushEmpty();
      return;
    case OT::Parenthesis:
      SharedTreeStack->pushParentheses();
      return PushPoincareExpression(exp.childAtIndex(0));
    case OT::JuniorExpression: {
      SharedTreeStack->clone(
          static_cast<Poincare::JuniorExpression&>(exp).tree());
      return;
    }
    default:
      assert(false);
  }
#endif
}

Tree* FromPoincareExpression(Poincare::OExpression exp) {
  Tree* e = Tree::FromBlocks(SharedTreeStack->lastBlock());
  PushPoincareExpression(exp);
  return e;
}

}  // namespace Poincare::Internal
