#ifndef POINCARE_LIST_COMPLEX_H
#define POINCARE_LIST_COMPLEX_H

#include <poincare/evaluation.h>
#include <poincare/list.h>

namespace Poincare {

template<typename T>
class ListComplex;

template<typename T>
class ListComplexNode final : public EvaluationNode<T> {
public:
  std::complex<T> complexAtIndex(int index) const override;
  int numberOfChildren() const override {
    return m_numberOfChildren < 0 ? 0 : m_numberOfChildren;
  }
  void incrementNumberOfChildren(int increment = 1) override {
    assert(!isUndefined() && m_numberOfChildren + increment >= 0);
    m_numberOfChildren+= increment;
  }
  void eraseNumberOfChildren() override { m_numberOfChildren = 0; }
  void setUndefined() {
    m_numberOfChildren = -1;
  }

  // TreeNode
  size_t size() const override { return sizeof(ListComplexNode<T>); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "ListComplex";
  }
  virtual void logAttributes(std::ostream & stream) const override {
    stream << " numberOfElements=\"" << m_numberOfChildren << "\"";
  }
#endif

  // EvaluationNode
  typename EvaluationNode<T>::Type type() const override { return EvaluationNode<T>::Type::ListComplex; }
  /* undef list is a list with a negative number of elements. */
  bool isUndefined() const override { return m_numberOfChildren < 0; }
  Expression complexToExpression(Preferences::Preferences::ComplexFormat complexFormat) const override;

private:
  int16_t m_numberOfChildren;

};

template<typename T>
class ListComplex final : public Evaluation<T> {
  friend class ListComplexNode<T>;
public:
  ListComplex() : Evaluation<T>(nullptr) {}
  ListComplex(ListComplexNode<T> * node) : Evaluation<T>(node) {}
  static ListComplex Builder() { return TreeHandle::NAryBuilder<ListComplex<T>, ListComplexNode<T>>(); }
  static ListComplex<T> Undefined();
  int numberOfChildren() const {
    return node()->numberOfChildren();
  }
  std::complex<T> complexAtIndex(int index) const {
    return node()->complexAtIndex(index);
  }
  void addChildAtIndexInPlace(Evaluation<T> t, int index, int currentNumberOfChildren);

  // Helper function
  ListComplex<T> sort();
private:
  ListComplexNode<T> * node() const { return static_cast<ListComplexNode<T> *>(Evaluation<T>::node()); }
};

}

#endif

