#ifndef POINCARE_EVALUATION_H
#define POINCARE_EVALUATION_H

#include <complex.h>
extern "C" {
#include <stdint.h>
}
#include <poincare/preferences.h>

#include "pool_handle.h"
#include "pool_object.h"

namespace Poincare {

class OExpression;
template <typename T>
class Evaluation;

template <typename T>
class EvaluationNode : public PoolObject {
 public:
  enum class Type : uint8_t {
    Exception,
    BooleanEvaluation,
    Complex,
    MatrixComplex,
    ListComplex,
    PointEvaluation
  };
  EvaluationNode<T> *childAtIndex(int index) const {
    return static_cast<EvaluationNode<T> *>(PoolObject::childAtIndex(index));
  }
  Direct<EvaluationNode<T>> children() const {
    return Direct<EvaluationNode<T>>(this);
  }
  virtual Type otype() const = 0;
  virtual ~EvaluationNode() = default;
  virtual bool isUndefined() const = 0;
  virtual std::complex<T> complexAtIndex(int index) const = 0;
  virtual T toScalar() const { return NAN; }
  virtual OExpression complexToExpression(
      Preferences::ComplexFormat complexFormat) const = 0;

  bool isDefinedScalar() const;
  bool isDefinedPoint() const;
  bool isListOfDefinedScalars() const;
  bool isListOfDefinedPoints() const;
};

template <typename T>
class Evaluation : public PoolHandle {
 public:
  Evaluation() : PoolHandle() {}
#if 0
  template<class U> U convert() const {
    /* This function allows to convert Evaluation to derived Evaluation.
     *
     * We could have overriden the operator T(). However, even with the
     * 'explicit' keyword (which prevents implicit casts), direct initilization
     * are enable which can lead to weird code:
     * ie, you can write: 'Complex<float> a(2); MatrixComplex<float> b(a);'
     * */
    static_assert(sizeof(U) == sizeof(Evaluation), "Size mismatch");
    return *reinterpret_cast<U *>(const_cast<Evaluation<T> *>(this));
  }
#endif
  EvaluationNode<T> *node() const {
    assert(!PoolHandle::object()->isGhost());
    return static_cast<EvaluationNode<T> *>(PoolHandle::object());
  }

  /* Hierarchy */
  Evaluation<T> childAtIndex(int i) const;
  typename Poincare::EvaluationNode<T>::Type otype() const {
    return node()->otype();
  }
  bool isUndefined() const { return node()->isUndefined(); }
  std::complex<T> complexAtIndex(int index) const {
    return node()->complexAtIndex(index);
  }
  T toScalar() const { return node()->toScalar(); }
  OExpression complexToExpression(
      Preferences::ComplexFormat complexFormat) const;

  bool isDefinedScalar() const { return node()->isDefinedScalar(); }
  bool isDefinedPoint() const { return node()->isDefinedPoint(); }
  bool isListOfDefinedScalars() const {
    return node()->isListOfDefinedScalars();
  }
  bool isListOfDefinedPoints() const { return node()->isListOfDefinedPoints(); }

 protected:
  Evaluation(EvaluationNode<T> *n) : PoolHandle(n) {}
};

}  // namespace Poincare

#endif
