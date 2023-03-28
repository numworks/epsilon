#ifndef POINCARE_POINT_EVALUATION_H
#define POINCARE_POINT_EVALUATION_H

#include "poincare/evaluation.h"

namespace Poincare {

template <typename T>
class PointEvaluationNode final : public EvaluationNode<T> {
 public:
  PointEvaluationNode(T x, T y) : m_x(x), m_y(y) {}

  // EvaluationNode
  typename EvaluationNode<T>::Type type() const override {
    return EvaluationNode<T>::Type::Point;
  }
  bool isUndefined() const override {
    return std::isnan(m_x) || std::isnan(m_y);
  }
  std::complex<T> complexAtIndex(int index) const override {
    return std::complex<T>(NAN, NAN);
  }
  Expression complexToExpression(
      Preferences::ComplexFormat complexFormat) const override;

  // TreeHandle
  size_t size() const override { return sizeof(PointEvaluationNode); }
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "PointEvaluation";
  }
  void logAttributes(std::ostream& stream) const override {
    stream << " x=\"" << m_x << "\"";
    stream << " y=\"" << m_y << "\"";
  }
#endif

 private:
  T m_x;
  T m_y;
};

template <typename T>
class PointEvaluation final : public Evaluation<T> {
 public:
  PointEvaluation(PointEvaluation<T>* node) : Evaluation<T>(node) {}
  static PointEvaluation Builder(T x, T y);
};

}  // namespace Poincare

#endif
