#ifndef POINCARE_EVALUATION_H
#define POINCARE_EVALUATION_H

#include <complex>
extern "C" {
#include <stdint.h>
}
#include <poincare/preferences.h>
#include <poincare/tree_node.h>
#include <poincare/tree_by_value.h>

namespace Poincare {

class Expression;
template<typename T>
class Evaluation;

template<typename T>
class EvaluationNode : public TreeNode {
public:
  enum class Type : uint8_t {
    AllocationFailure,
    Complex,
    MatrixComplex
  };
  EvaluationNode<T> * childAtIndex(int index) const override { return static_cast<EvaluationNode<T> *>(TreeNode::childAtIndex(index)); }
  virtual Type type() const = 0;
  virtual ~EvaluationNode() = default;
  virtual bool isUndefined() const = 0;
  virtual T toScalar() const { return NAN; }
  virtual Expression complexToExpression(Preferences::ComplexFormat complexFormat) const = 0;
  virtual std::complex<T> trace() const = 0;
  virtual std::complex<T> determinant() const = 0;
  virtual Evaluation<T> inverse() const = 0;
  virtual Evaluation<T> transpose() const = 0;

  // TreeNode
  static TreeNode * FailedAllocationStaticNode();
  TreeNode * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }
};

template<typename T>
class Evaluation : public TreeByValue {
public:
  EvaluationNode<T> * node() const override {
    assert(!TreeByValue::node().isGhost());
    return static_cast<EvaluationNode<T> *>(TreeByValue::node());
  }
  typename Poincare::EvaluationNode<T>::Type type() const { return node()->type(); }
  bool isUndefined() const { return node()->isUndefined(); }
  T toScalar() const { return node()->toScalar(); }
  Expression complexToExpression(Preferences::ComplexFormat complexFormat) const;
  std::complex<T> trace() const { return node()->trace(); }
  std::complex<T> determinant() const { return node()->determinant(); }
  Evaluation inverse() const { return node()->inverse(); }
  Evaluation transpose() const { return node()->transpose(); }
protected:
  //Evaluation(EvaluationNode * n) : TreeByValue(n) {}
  //Evaluation() : TreeByValue() {}
};

}

#endif
