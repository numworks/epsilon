#ifndef POINCARE_EVALUATION_H
#define POINCARE_EVALUATION_H

#include <complex>
extern "C" {
#include <stdint.h>
}
#include <poincare/preferences.h>
#include <poincare/tree_node.h>
#include <poincare/tree_reference.h>

namespace Poincare {

class ExpressionReference;
template<typename T>
class EvaluationReference;

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
  virtual ExpressionReference complexToExpression(Preferences::ComplexFormat complexFormat) const = 0;
  virtual std::complex<T> trace() const = 0;
  virtual std::complex<T> determinant() const = 0;
  virtual EvaluationReference<T> inverse() const = 0;
  virtual EvaluationReference<T> transpose() const = 0;

  // TreeNode
  TreeNode * ghostStaticNode() override {
    assert(false);
    return FailedAllocationStaticNode();
  }

  static TreeNode * FailedAllocationStaticNode();
  TreeNode * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }

  // Tree
  //LayoutNode * childAtIndex(int i) { return static_cast<LayoutNode *>(TreeNode::childAtIndex(i)); }
};

template<typename T>
class EvaluationReference : public TreeReference {
public:
  EvaluationReference(TreeNode * n, bool isCreatingNode = false) : TreeReference(n, isCreatingNode) {}
  EvaluationNode<T> * node() const override{ return static_cast<EvaluationNode<T> *>(TreeReference::node()); }
  typename Poincare::EvaluationNode<T>::Type type() const { return node()->type(); }
  bool isUndefined() const { return node()->isUndefined(); }
  T toScalar() const { return node()->toScalar(); }
  ExpressionReference complexToExpression(Preferences::ComplexFormat complexFormat) const;
  std::complex<T> trace() const { return node()->trace(); }
  std::complex<T> determinant() const { return node()->determinant(); }
  EvaluationReference inverse() const { return node()->inverse(); }
  EvaluationReference transpose() const { return node()->transpose(); }
protected:
  EvaluationReference() : TreeReference() {}
};

}

#endif
