#include "approximation.h"
#include "constant.h"
#include "rational.h"
#include <poincare_junior/src/memory/node_iterator.h>

namespace Poincare {

template<typename T>
T Approximation::To(const TypeBlock * block) {
  if (block->isRational()) {
    return Rational::Numerator(block).to<T>() / Rational::Denominator(block).to<T>();
  }
  switch (block->type()) {
    case BlockType::Constant:
      return Constant::To<T>(static_cast<Constant::Type>(static_cast<uint8_t>(*block->next())));
    case BlockType::Addition:
      return Approximation::MapAndReduce(block, FloatAddition<T>);
    case BlockType::Multiplication:
      return Approximation::MapAndReduce(block, FloatMultiplication<T>);
    case BlockType::Division:
      return Approximation::MapAndReduce(block, FloatDivision<T>);
    case BlockType::Subtraction:
      return Approximation::MapAndReduce(block, FloatSubtraction<T>);
    default:
      assert(false);
  };
}

template<typename T>
T Approximation::MapAndReduce(const TypeBlock * block, Reductor<T> reductor) {
  T res;
  for (std::pair<Node, int> indexedNode : NodeIterator::Children<Forward, NoEditable>(Node(block))) {
    T app = Approximation::To<T>(std::get<Node>(indexedNode).block());
    if (std::get<int>(indexedNode) == 0) {
      res = app;
    } else {
      res = reductor(res, app);
    }
  }
  return res;
}

}

template float Poincare::Approximation::To<float>(Poincare::TypeBlock const*);
template double Poincare::Approximation::To<double>(Poincare::TypeBlock const*);
