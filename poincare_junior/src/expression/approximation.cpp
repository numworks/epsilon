#include "approximation.h"
#include "constant.h"
#include "rational.h"
#include <poincare_junior/src/memory/node_iterator.h>

namespace Poincare {

template<typename T>
T Approximation::To(const Node node) {
  if (node.block()->isRational()) {
    return Rational::Numerator(node).to<T>() / Rational::Denominator(node).to<T>();
  }
  switch (node.type()) {
    case BlockType::Constant:
      return Constant::To<T>(Constant::Type(node));
    case BlockType::Float:
      return *reinterpret_cast<float *>(node.block()->next());
    case BlockType::Addition:
      return Approximation::MapAndReduce(node, FloatAddition<T>);
    case BlockType::Multiplication:
      return Approximation::MapAndReduce(node, FloatMultiplication<T>);
    case BlockType::Division:
      return Approximation::MapAndReduce(node, FloatDivision<T>);
    case BlockType::Subtraction:
      return Approximation::MapAndReduce(node, FloatSubtraction<T>);
    default:
      assert(false);
  };
}

template<typename T>
T Approximation::MapAndReduce(const Node node, Reductor<T> reductor) {
  T res;
  for (std::pair<Node, int> indexedNode : NodeIterator::Children<Forward, NoEditable>(node)) {
    T app = Approximation::To<T>(std::get<Node>(indexedNode));
    if (std::get<int>(indexedNode) == 0) {
      res = app;
    } else {
      res = reductor(res, app);
    }
  }
  return res;
}

}

template float Poincare::Approximation::To<float>(const Poincare::Node);
template double Poincare::Approximation::To<double>(const Poincare::Node);
