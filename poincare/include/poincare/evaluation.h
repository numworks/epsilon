#ifndef POINCARE_EVALUATION_H
#define POINCARE_EVALUATION_H

#include <complex.h>
extern "C" {
#include <stdint.h>
}
#include <poincare/preferences.h>
#include <poincare/tree_handle.h>
#include <poincare/tree_node.h>

namespace Poincare {

class Expression;
template<typename T>
class Evaluation;

template<typename T>
class EvaluationNode : public TreeNode {
public:
  enum class Type : uint8_t {
    Exception,
    Complex,
    MatrixComplex,
    ListComplex
  };
  EvaluationNode<T> * childAtIndex(int index) const { return static_cast<EvaluationNode<T> *>(TreeNode::childAtIndex(index)); }
  Direct<EvaluationNode<T> > children() const { return Direct<EvaluationNode<T> >(this); }
  virtual Type type() const = 0;
  virtual ~EvaluationNode() = default;
  virtual bool isUndefined() const = 0;
  virtual std::complex<T> complexAtIndex(int index) const = 0;
  virtual T toScalar() const { return NAN; }
  virtual Expression complexToExpression(Preferences::ComplexFormat complexFormat) const = 0;
};

template<typename T>
class Evaluation : public TreeHandle {
public:
  Evaluation() : TreeHandle() {}
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
  EvaluationNode<T> * node() const {
    assert(!TreeHandle::node()->isGhost());
    return static_cast<EvaluationNode<T> *>(TreeHandle::node());
  }

  /* Hierarchy */
  Evaluation<T> childAtIndex(int i) const;
  typename Poincare::EvaluationNode<T>::Type type() const { return node()->type(); }
  bool isUndefined() const { return node()->isUndefined(); }
  std::complex<T> complexAtIndex(int index) const { return node()->complexAtIndex(index); }
  T toScalar() const { return node()->toScalar(); }
  Expression complexToExpression(Preferences::ComplexFormat complexFormat) const;

protected:
  Evaluation(EvaluationNode<T> * n) : TreeHandle(n) {}
};

}

#endif
