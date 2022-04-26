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
  std::complex<T> complexAtIndex(int index) const;
  int numberOfChildren() const override { return m_numberOfChildren; }
  void incrementNumberOfChildren(int increment = 1) override { m_numberOfChildren+= increment; }
  void decrementNumberOfChildren(int decrement = 1) override {
    assert(m_numberOfChildren >= decrement);
    m_numberOfChildren-= decrement;
  }
  void eraseNumberOfChildren() override { m_numberOfChildren = 0; }

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
  /* undef lists do not exist (yet), since {undef} is not an undefined list,
   * but a list of undefined elements. */
  bool isUndefined() const override { return false; }
  Expression complexToExpression(Preferences::Preferences::ComplexFormat complexFormat) const override;
private:
  uint16_t m_numberOfChildren;

};

template<typename T>
class ListComplex final : public Evaluation<T> {
  friend class ListComplexNode<T>;
public:
  ListComplex(ListComplexNode<T> * node) : Evaluation<T>(node) {}
  static ListComplex Builder() { return TreeHandle::NAryBuilder<ListComplex<T>, ListComplexNode<T>>(); }
  std::complex<T> complexAtIndex(int index) const {
    return node()->complexAtIndex(index);
  }
  void addChildAtIndexInPlace(Evaluation<T> t, int index, int currentNumberOfChildren);
private:
  ListComplexNode<T> * node() const { return static_cast<ListComplexNode<T> *>(Evaluation<T>::node()); }
};

}

#endif

